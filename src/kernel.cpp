#include "kernel.h"
#include "easyopencl.h"

#include <iostream>
#include <sstream>
#include <utility>
#include <fstream>

/**
 * Load the kernel from disk
 *
 * Input:   std::string filename
 * Output:  void
 *
 * Effect:  * Reads the file specified by the input filename into a string
 *          * Create a cl_program object from the source and build it
 *          * Create a kernel from the compiled code and store it
 */
template<typename T>
 Kernel<T>::Kernel(std::string id_, cl_context& context_, cl_command_queue& commandQueue_
                  , cl_device_id* devices, std::string filename, EasyOpenCL<T>* framework_ ) {

  //Assign the captured variables
  id = id_;
  context = context_;
  commandQueue = commandQueue_;
  framework = framework_;

  // Open the file
  std::ifstream f(filename);
  if (!f.good()) {
    raiseError("Unable to open kernel file: " + filename);
  }

  // Store the file contents as a std::string using a std::stringstream
  std::stringstream buffer;
  buffer << f.rdbuf();
  std::string fileContents = buffer.str();

  // Convert it to a C-style string
  const char *source = fileContents.c_str();
  const size_t length = fileContents.length();

  // Create a cl_program object from the source code string
  cl_program program = clCreateProgramWithSource(context, 1, &source, &length, &status);
  checkError("clCreateProgramWithSource");

  // Build the program file into an object file
  status = clBuildProgram(program, 1, devices, NULL, NULL, NULL);

  // On failure, allocate a buffer, fill it with the error message and display it
  if(status != CL_SUCCESS) {
    char buffer[10240];
    clGetProgramBuildInfo(program, devices[0], CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, NULL);
    std::cerr << buffer << std::endl;
  }
  checkError("clBuildProgram");

  // Create a kernel from the built program
  // The kernel name is the same as the filename, without the extension
  // This name should match the entry function in the file
  std::string kernelName_file = filename.substr(0, filename.find('.'));

  kernel = clCreateKernel(program, kernelName_file.c_str(), &status);
  if(status != CL_SUCCESS) {

    std::cerr << "Make sure that the name of the entry function in '"
    << filename << "'' is equal to '"
    << kernelName_file << "'" << std::endl;
  }
  checkError("clCreateKernel");

  // Clean up the object files of the program now we have obtained the kernel
  status = clReleaseProgram(program);
  checkError("clReleaseProgram");
}

/******************************************************************************/
//  BINDING VALUES TO THE BUFFERS
/******************************************************************************/
/**
 * Add an input buffer to the kernel
 *
 * Input:   int argPos  - the position of the argument
 *          std::vector<T> input  - the boundValues of the kernel input
 */
template<typename T>
void Kernel<T>::bindInput(uint argPos, std::vector<T> input) {

  vectorSize = input.size();

  if(framework->getVectorSize() == -1) {
    framework->setVectorSize(vectorSize);
  }


  // Create the actual input buffer at the designated postion
  // Getting the boundValues from the vector involves from C-style hacking
  // It passes a pointer to the first element in the vector - so this does assume
  // that all elements are sequentially aligned in memory
  cl_mem inputBuffer = clCreateBuffer(context
    , CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR
    , input.size() * sizeof(T)
    , (void*)&input[0]
    , NULL);

  status = clSetKernelArg(kernel
    , argPos
    , sizeof(cl_mem)
    , (void*)&inputBuffer );

  checkError("clSetKernelArg input " + std::to_string(argPos));

  // Add the buffer to the map for later reference - retrieval and cleanup
  erase(argPos);
  boundBuffers.emplace(argPos, BoundBuffer(inputBuffer, input.size()));
}

/**
 * Add an output buffer to the kernel
 *
 * Input:   int argPos  - the position of the argument
 */

/*
  Buffer length check priority:
  1. passed as an argument
  2. the value of the kernel
  3. the value of the framework
 */
template<typename T>
void Kernel<T>::bindOutput(uint argPos) {

  // The program needs to know the length of the buffer - therefore, first pass
  // an input buffer so the length can be determined
  uint bufferSize;

  if (vectorSize != -1)
  {
    //from the kernel
    bufferSize = vectorSize;
  }
  else if (framework->getVectorSize() != -1)
  {
    //from the framework
    bufferSize = framework->getVectorSize();
  }
  else
  {
    raiseError("Unable to determine output buffer size.");
  }

  bindOutput(argPos, bufferSize);
}

template<typename T>
void Kernel<T>::bindOutput(uint argPos, uint bufferSize) {

  // Create and append the actual output buffer
  cl_mem outputBuffer = clCreateBuffer(context, CL_MEM_READ_WRITE, bufferSize * sizeof(T), NULL, NULL);
  status = clSetKernelArg(kernel, argPos, sizeof(cl_mem), (void *)&outputBuffer);
  checkError("clSetKernelArg outputBuffer " + std::to_string(argPos));

  // Add the buffer to the map for later reference - retrieval and cleanup
  erase(argPos);
  boundBuffers.emplace(argPos, BoundBuffer(outputBuffer, bufferSize));
}

template<typename T>
void Kernel<T>::bindPromise(Kernel<T>& sourceKernel, uint sourceArgPos, uint argPos) {
  erase(argPos);
  boundPromises.emplace(argPos, BoundPromise<T>(&sourceKernel, sourceArgPos, argPos));
}


/******************************************************************************/
//  MANAGING THE BOUNDVALUE MAPS
/******************************************************************************/
template<typename T>
void Kernel<T>::erase(uint argPos) {
  boundScalars.erase(argPos);
  boundBuffers.erase(argPos);
  boundPromises.erase(argPos);
}

/*******************************************************/
//  RUNNING A KERNEL
/*******************************************************/
template<typename T>
void Kernel<T>::runKernel() {

  if(debug) {
    std::cout << "Attempting to execute '" << id << "'." << std::endl;
  }

  // Check whether the buffers specified have all been specified
  cl_uint kernelNumArgs;
  status = clGetKernelInfo(kernel, CL_KERNEL_NUM_ARGS, sizeof(cl_uint), &kernelNumArgs, NULL);


  uint totalBoundArguments = boundScalars.size() + boundBuffers.size() + boundPromises.size();

  if(kernelNumArgs != totalBoundArguments) {
    raiseError("You have only specified " + std::to_string(totalBoundArguments) + "/" + std::to_string(kernelNumArgs) + " arguments for kernel '" + id + "'. (TODO, which ones are lacking?");
  }

  //Check whether there are dependencies
  if(boundPromises.size()) {
    // Dependencies exist, find them and resolve them by executing them
    for(auto& kv : boundPromises)
    {
        //uint argPos = kv.first;
        BoundPromise<T>& promise = kv.second;
        Kernel<T> * sourceKernel = promise.sourceKernel;
        std::string sourceId = sourceKernel->getId();

        std::cout << "Found dependency\t" <<
        sourceId << "(" << promise.sourceArgPos << ") -> " <<
        id << "(" << promise.targetArgPos << ")" << std::endl;

        if(sourceKernel->getExecutionCount() == 0) {

          if(debug) {
            std::cout << sourceId << " has not been executed yet. Attempting to run!" << std::endl;
          }

          //Run the kernel (this can trigger more dependencies)
          sourceKernel->runKernel();

          BoundBuffer& buf = sourceKernel->boundBuffers.find(promise.sourceArgPos)->second;
          cl_mem& memObject = buf.getMemObject();

          //Set the kernel arguments of the current kernel
          status = clSetKernelArg(kernel      // change the current kernel
                    , promise.targetArgPos    // bind to the port that depends
                    , sizeof(cl_mem)
                    , (void*)&memObject);  // the cl_mem object from the output

          checkError("Added output buffer already present on GPU to dependent kernel '" + id + "'");

        } else {
          if(debug) {
            std::cout << sourceId << " has been executed already!" << std::endl;
          }
        }
      }
  }
  else {
    if(debug) {
      std::cout << "No kernel dependencies found." << std::endl;
    }
  }



  /*
  // Get the kernel max work group size
  size_t maxWorkGroupSize;
  status = clGetKernelWorkGroupInfo(kernel, NULL, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &maxWorkGroupSize, NULL);
  std::cout << maxWorkGroupSize << std::endl;
  */


  // Create a global_work_size array
  // This determines how many workers will execute the kernel
  // If you set a single worker, the process will be sequential
  // If you set too many workers, the OpenCL driver will be unable to function
  // Determine the values of the work group sizes
  //
  //

  if(vectorSize == -1) {
    vectorSize = framework->getVectorSize();
  }

  size_t global_work_size[] = { vectorSize };
  size_t local_work_size[] = { vectorSize };

  // Invoke the actual kernel execution
  status = clEnqueueNDRangeKernel(  commandQueue
          , kernel
          , 1               // The work dimension (1, 2 or 3)
          , NULL            // global_work_offset (must be NULL)
          , global_work_size
          , local_work_size
          , 0               // amount of events needing completion before this
          , NULL            // event wait list
          , NULL );         // pointer to a event object for this execution

  checkError("Running kernel " + id);

  executionCounter++;

  std::cout << "Executed '" << id << "'." << std::endl;
}

/*******************************************************/
//  RETRIEVING VALUES FROM THE BUFFERS
/*******************************************************/
/**
 * Retrieve a value after running the kernel
 *
 * Input:   uint argumentPosition - which argument should be fetched?
 *
 * Output:  std::vector<T>        - containing the values of the argument
 */
template<typename T>
std::vector<T> Kernel<T>::getBuffer(uint argPos) {

  // Check whether the argument was actually part of the kernel
  auto itBuffer = boundBuffers.find(argPos);
  auto itPromise = boundPromises.find(argPos);

  if(itBuffer == boundBuffers.end() && itPromise == boundPromises.end()) {
    raiseError("The buffer at position " + std::to_string(argPos) + " could not be retrieved");
  }

  uint size;
  cl_mem bufferHandle;

  if(itBuffer != boundBuffers.end()) {
    // The found buffer is an actual one
    size = itBuffer->second.getSize();
    bufferHandle = itBuffer->second;

  } else {
    uint pos = itPromise->second.sourceArgPos;
    bufferHandle = itPromise->second.sourceKernel->boundBuffers.at(pos);
    size = itPromise->second.sourceKernel->boundBuffers.at(pos).getSize();
  }

  T * hostBuffer = new T[size];

  // Read the values from the OpenCL device into the buffer

  status = clEnqueueReadBuffer( commandQueue
    , bufferHandle
    , CL_TRUE
    , 0
    , size * sizeof(T)
    , hostBuffer
    , 0
    , NULL
    , NULL );

  //Clean up the buffer and raise an error if something went wrong
  if (status != CL_SUCCESS) {
    delete hostBuffer;
    raiseError("clEnqueueReadBuffer" + '\t' + getErrorString(status));
  }

  // Element by element - copy the boundValues into the vector
  std::vector<T> hostVector {};
  hostVector.reserve(size);

  for (unsigned i = 0; i < size; i++) {
    hostVector.push_back(hostBuffer[i]);
  }

  //Clean up the hostBuffer
  delete hostBuffer;
  return hostVector;
}

/**
 * Utility function for pretty-printing the contents of a buffer
 *
 * Input:   uint argumentPosition
 */
template<typename T>
void Kernel<T>::showBuffer(uint argPos) {

  std::vector<T> output = getBuffer(argPos);

  std::cout << "[ ";
    for (unsigned i = 0; i < output.size(); i++) {
      std::cout << output[i];

      if (i != output.size() - 1) {
        std::cout << ", ";
      }
    }
    std::cout << " ]" << std::endl;
}

/**
 * Utility function for pretty-printing the contents of a buffer
 *
 * Input:   uint argumentPosition
 */
template<typename T>
void Kernel<T>::showBuffers() {
  for(auto& kv : boundBuffers) {
    std::cout << kv.first << " : ";
    showBuffer(kv.first);
  }
}

template<typename T>
void Kernel<T>::releaseMemObjects() {
  for (auto& kv : boundBuffers) {
    status = clReleaseMemObject(kv.second);
    checkError("clReleaseMemObject");
  }
}

template<typename T>
Kernel<T>::operator cl_kernel() {
  return kernel;
}


template class Kernel<float>;
// template class Kernel<int>;
// template class Kernel<double>;

// template void Kernel<int>::bindScalar<int>(uint,int);
// template void Kernel<int>::bindScalar<float>(uint,float);
// template void Kernel<int>::bindScalar<double>(uint,double);

// template void Kernel<float>::bindScalar<int>(uint,int);
// template void Kernel<float>::bindScalar<float>(uint,float);
// template void Kernel<float>::bindScalar<double>(uint,double);

// template void Kernel<double>::bindScalar<int>(uint,int);
// template void Kernel<double>::bindScalar<float>(uint,float);
// template void Kernel<double>::bindScalar<double>(uint,double);
