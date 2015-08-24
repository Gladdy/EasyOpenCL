#include "kernel.h"

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
Kernel::Kernel(std::string id_, cl_context& context_, cl_device_id* devices, std::string filename) {

  //Assign the captured variables
  id = id_;
  context = context_;

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
 *          std::vector<T> input  - the values of the kernel input
 */
template<typename T>
void Kernel::setInputBuffer(uint argPos, std::vector<T> input) {

  // Make sure that all input vectors have the same length
  // This allows for easier kernel writing but might be inefficient
  // For passing single values to a kernel, just add another buffer and place
  // the values in the vefctor and access them normally
  // if (vectorSize != -1 && vectorSize != input.size())  {
  //   raiseError("You passed vectors with different lengths to the framework");
  // }
  // vectorSize = input.size();
  // if (vectorSize == -1)  { raiseError("Please pass the input buffer first"); }

  // Create the actual input buffer at the designated postion
  // Getting the values from the vector involves from C-style hacking
  // It passes a pointer to the first element in the vector - so this does assume
  // that all elements are sequentially aligned in memory
  cl_mem inputBuffer = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, vectorSize * sizeof(T), (void *)&input[0], NULL);
  status = clSetKernelArg(kernel, argPos, sizeof(cl_mem), (void *)&inputBuffer);
  checkError("clSetKernelArg input " + std::to_string(argPos));

  // Add the buffer to the map for later reference - retrieval and cleanup
  boundValues[argPos] = BoundValue<T>(inputBuffer);
}

/**
 * Add an output buffer to the kernel
 *
 * Input:   int argPos  - the position of the argument
 */

template<typename T>
void Kernel::setOutputBuffer(uint argPos) {

  // The program needs to know the length of the buffer - therefore, first pass
  // an input buffer so the length can be determined
  // if (vectorSize == -1)  { raiseError("Please pass the input buffer first"); }
  // if (kernels.count(id) == 0) { raiseError("No kernel by id '" + id +"' exists"); }

  // Create and append the actual output buffer
  cl_mem outputBuffer = clCreateBuffer(context, CL_MEM_READ_WRITE, vectorSize * sizeof(T), NULL, NULL);
  status = clSetKernelArg(kernel, argPos, sizeof(cl_mem), (void *)&outputBuffer);
  checkError("clSetKernelArg outputBuffer " + std::to_string(argPos));

  // Add the buffer to the map for later reference - retrieval and cleanup
  boundValues[argPos] = BoundValue<T>(outputBuffer);
}

template<typename T>
void Kernel::setSingleValue(uint argPos, T value) {

  //if(kernels.count(id) == 0) { raiseError("No kernel by id '" + id +"' exists"); }

  // See setInputBuffer comments
  status = clSetKernelArg(kernel, argPos, sizeof(T), &value);
  checkError("clSetKernelArg singleValue " + std::to_string(argPos));

  // Add the buffer to the map for later reference - retrieval and cleanup
  boundValues[argPos] = BoundValue<T>(value);
}

/******************************************************************************/
//  RETRIEVING THE VALUES
/******************************************************************************/
template<typename T>
std::vector<T> Kernel::getValue(uint argPost) {

  // Check whether the argument was actually part of the kernel
  //typename std::map<uint,BoundValue>::iterator it;

  auto it = values.find(argumentPosition);
  if(it == values.end()) {
    raiseError("The buffer at position " + std::to_string(argumentPosition) + " could not be retrieved");
  }

  std::vector<T> hostOutputVector;

  if(it->second.isScalar()) {
    hostOutputVector.push_back(it->second.getScalarValue());
  }
  else
  {
    // Allocate a buffer which can hold the results
    // Copy the values into said buffer
    T* hostOutputBuffer = new T[vectorSize];
    status = clEnqueueReadBuffer(commandQueue, it->second, CL_TRUE, 0, vectorSize * sizeof(T), hostOutputBuffer, 0, NULL, NULL);
    if (status != CL_SUCCESS) {
      //Clean up the buffer and raise an error if something went wrong
      delete hostOutputBuffer;
      raiseError("clEnqueueReadBuffer" + '\t' + getErrorString(status));
    }

    // Create the final result vector
    // Element by element - copy the values into the vector
    hostOutputVector.reserve(vectorSize);
    for (unsigned i = 0; i < vectorSize; i++) {
      hostOutputVector.push_back(hostOutputBuffer[i]);
    }
    delete hostOutputBuffer;
  }

  return hostOutputVector;
}

void Kernel::releaseMemObjects() {
  for (auto& kv : boundValues) {
    status = clReleaseMemObject(kv.second);
    checkError("clReleaseMemObject");
  }
}

Kernel::operator cl_kernel() {
  return kernel;
}


template Kernel::setInputBuffer<int>(uint, std::vector<int>);
template Kernel::setOutputBuffer<int>(uint);
template Kernel::setSingleValue<int>(uint,int);
template Kernel::getValue<int>(uint);
