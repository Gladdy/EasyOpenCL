#include "easyopencl.h"

#include <iostream>
#include <exception>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <utility>
#include <algorithm>

template<class T>
void EasyOpenCL<T>::raiseError(std::string errorString) {
  throw std::runtime_error(errorString.c_str());
}

template<class T>
void EasyOpenCL<T>::checkError(std::string errorLocation) {
  if (status != CL_SUCCESS)
  {
    raiseError(errorLocation + '\t' + getErrorString(status));
  }
}
/**
 * Construct an EasyOpenCL object
 *
 * Input:   bool printData - sets debug verbosity for the framework
 *
 * Effects: * Select the first platform available
 *          * Chooses a device (first choice: GPU, fallback: CPU)
 *          * If debug verbosity is enabled: print the selected device info
 *          * Create an OpenCL context and an OpenCL CommandQueue
 */
template<class T>
EasyOpenCL<T>::EasyOpenCL(bool printData) {

  info = printData;
  cl_uint numPlatforms;           //the NO. of platforms

  // Fetch the different platforms on which we can run our kernel
  cl_platform_id platform = NULL;
  status = clGetPlatformIDs(0, NULL, &numPlatforms);
  checkError("clGetPlatformIDs");

  // Take the first platform available
  if (numPlatforms > 0)
  {
    cl_platform_id* platforms = (cl_platform_id*) malloc(numPlatforms * sizeof(cl_platform_id));
    status = clGetPlatformIDs(numPlatforms, platforms, NULL);
    platform = platforms[0];
    free(platforms);
  }

  // Get the devices which are available on said platform
  cl_uint numDevices = 0;
  status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 0, NULL, &numDevices);

  if (numDevices)
  {
    //Use the first GPU available
    devices = (cl_device_id*)malloc(numDevices * sizeof(cl_device_id));
    status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, numDevices, devices, NULL);
  }
  else
  {
    // If there is no GPU support, fall back to the CPU

    if(info) {
      std::cout << "No supported GPU device available." << std::endl;
      std::cout << "Falling back to using the CPU." << std::endl;
      std::cout << std::endl;
    }

    status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 0, NULL, &numDevices);
    devices = (cl_device_id*)malloc(numDevices * sizeof(cl_device_id));
    status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, numDevices, devices, NULL);
  }

  //Print the data of the selected device
  if (info) {
    printDeviceProperty(*devices);
  }

  //Create an OpenCL context and a command queue
  context = clCreateContext(NULL, 1, devices, NULL, NULL, &status);
  checkError("clCreateContext");

  commandQueue = clCreateCommandQueueWithProperties(context, devices[0], 0, &status);
  checkError("clCreateCommandQueueWithProperties");
}

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
template<class T>
void EasyOpenCL<T>::loadKernel(std::string filename) {

  //Open the file
  std::ifstream f(filename);
  if (!f.good()) {
    raiseError("Unable to open kernel file: " + filename);
  }

  //Store the file contents as a std::string using a std::stringstream
  std::stringstream buffer;
  buffer << f.rdbuf();
  std::string fileContents = buffer.str();

  //Convert the std::string to a C-style string for the OpenCL program creation
  const char *source = fileContents.c_str();
  size_t sourceSize = fileContents.length();

  //Create a cl_program object from the source code string
  cl_program program;
  program = clCreateProgramWithSource(context, 1, &source, &sourceSize, &status);
  checkError("clCreateProgramWithSource");

  //Build the program file into an object file
  status = clBuildProgram(program, 1, devices, NULL, NULL, NULL);

  //On failure, allocate a buffer, fill it with the error message and display it
  if(status != CL_SUCCESS) {
    char buffer[10240];
    clGetProgramBuildInfo(program, devices[0], CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, NULL);
    std::cerr << buffer << std::endl;
  }
  checkError("clBuildProgram");

  // Create a kernel from the built program
  // The kernel name is the same as the filename, without the extension
  // This name should match the entry function in the file
  kernelName = filename.substr(0, filename.find('.'));
  kernel = clCreateKernel(program, kernelName.c_str(), &status);
  if(status != CL_SUCCESS) {
    std::cerr << "Make sure that the name of the entry function in '"
              << filename << "'' is equal to '"
              << kernelName << "'." << std::endl;
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
 * Input:   int argumentPosition  - the position of the argument
 *          std::vector<T> input  - the values of the kernel input
 */
template<typename T>
void EasyOpenCL<T>::setInputBuffer(uint argumentPosition, std::vector<T> input) {

  // Make sure that all input vectors have the same length
  // This allows for easier kernel writing but might be inefficient
  // For passing single values to a kernel, just add another buffer and place
  // the values in the vector and access them normally
  if (vectorSize != -1 && vectorSize != input.size())  {
    raiseError("You passed vectors with different lengths to the framework");
  }
  vectorSize = input.size();

  // Create the actual input buffer at the designated postion
  // Getting the values from the vector involves from C-style hacking
  // It passes a pointer to the first element in the vector - so this does assume
  // that all elements are sequentially aligned in memory
  cl_mem inputBuffer = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, vectorSize * sizeof(T), (void *)&input[0], NULL);
  status = clSetKernelArg(kernel, argumentPosition, sizeof(cl_mem), (void *)&inputBuffer);
  checkError("clSetKernelArg input " + std::to_string(argumentPosition));

  // Add the buffer to the map for later reference - retrieval and cleanup
  values[argumentPosition] = BoundValue<T>(inputBuffer);
}

/**
 * Add an output buffer to the kernel
 *
 * Input:   int argumentPosition  - the position of the argument
 */
template<typename T>
void EasyOpenCL<T>::setOutputBuffer(uint argumentPosition) {

  // The program needs to know the length of the buffer - therefore, first pass
  // an input buffer so the length can be determined
  if (vectorSize == -1)  { raiseError("Please pass the input buffer first"); }

  // Create and append the actual output buffer
  cl_mem outputBuffer = clCreateBuffer(context, CL_MEM_READ_WRITE, vectorSize * sizeof(T), NULL, NULL);
  status = clSetKernelArg(kernel, argumentPosition, sizeof(cl_mem), (void *)&outputBuffer);
  checkError("clSetKernelArg outputBuffer " + std::to_string(argumentPosition));

  // Add the buffer to the map for later reference - retrieval and cleanup
  values[argumentPosition] = BoundValue<T>(outputBuffer);
}

template<typename T>
void EasyOpenCL<T>::setSingleValue(uint argumentPosition, T value) {

  // See setInputBuffer comments
  status = clSetKernelArg(kernel, argumentPosition, sizeof(T), &value);
  checkError("clSetKernelArg singleValue " + std::to_string(argumentPosition));

  // Add the buffer to the map for later reference - retrieval and cleanup
  values[argumentPosition] = BoundValue<T>(value);
}

/******************************************************************************/
//  EXECUTING THE KERNEL
/******************************************************************************/

/**
 * Run the kernel!
 */
template<class T>
void EasyOpenCL<T>::runKernel() {

  // Check whether the buffers specified have all been specified
  cl_uint kernelNumArgs;
  status = clGetKernelInfo(kernel, CL_KERNEL_NUM_ARGS, sizeof(cl_uint), &kernelNumArgs, NULL);

  // Get the kernel max work group size
  // size_t maxWorkGroupSize;
  // status = clGetKernelWorkGroupInfo(kernel, NULL, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &maxWorkGroupSize, NULL);
  // std::cout << maxWorkGroupSize << std::endl;

  std::vector<uint> argumentVector;
  for(auto& kv : values) {
    argumentVector.push_back(kv.first);
  }

  std::sort(argumentVector.begin(), argumentVector.end());

  // for(uint i = 0; i < kernelNumArgs; i++) {
  //   if(i > (argumentVector.size()-1) || argumentVector[i] != i) {
  //     raiseError("The buffers do not form a sequential set starting at 0: " + std::to_string(i) + " is missing");
  //   }
  // }

  // Create a global_work_size array
  // This determines how many workers will execute the kernel
  // If you set a single worker, the process will be sequential
  // If you set too many workers, the OpenCL driver will be unable to function
  size_t global_work_size[] = { vectorSize };
  size_t local_work_size[] = { vectorSize };
  status = clEnqueueNDRangeKernel(commandQueue, kernel, 1, NULL, global_work_size, local_work_size, 0, NULL, NULL);
  checkError("Running kernel");
}



/**
 * Retrieve a value after running the kernel
 *
 * Input:   uint argumentPosition - which argument should be fetched?
 *
 * Output:  std::vector<T>        - containing the values of the argument
 */
template<typename T>
std::vector<T> EasyOpenCL<T>::getValue(uint argumentPosition) {

  // Check whether the argument was actually part of the kernel
  typename std::map<uint,BoundValue<T>>::iterator it;
  it = values.find(argumentPosition);
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

/**
 * Utility function for pretty-printing the contents of a buffer
 *
 * Input:   uint argumentPosition
 */
template<typename T>
void EasyOpenCL<T>::showValue(uint argumentPosition) {

  std::vector<T> output = getValue(argumentPosition);

  std::cout << "[ ";
  for (unsigned i = 0; i < output.size(); i++) {
    std::cout << output[i];

    if (i != output.size() - 1) {
      std::cout << ", ";
    }
  }
  std::cout << " ]" << std::endl;
}

template<typename T>
void EasyOpenCL<T>::showAllValues() {
  for(auto& kv : values) {
    std::cout << kv.first << " : ";
    showValue(kv.first);
  }
}


template<typename T>
void EasyOpenCL<T>::cleanup() {
  status = clReleaseKernel(kernel);
  checkError("clReleaseKernel");

  for (auto& kv : values) {
    status = clReleaseMemObject(kv.second);
    checkError("clReleaseMemObject");
  }

  status = clReleaseCommandQueue(commandQueue);
  checkError("clReleaseCommandQueue");
  status = clReleaseContext(context);
  checkError("clReleaseContext");

  if (devices != NULL)
  {
    free(devices);
    devices = NULL;
  }
}

template<typename T>
void EasyOpenCL<T>::printDeviceProperty(cl_device_id device) {

  /*
  from: http://dhruba.name/2012/08/14/opencl-cookbook-listing-all-devices-and-their-critical-attributes/
  */
  char* value;
  size_t valueSize;
  cl_uint maxComputeUnits;

  // print device name
  clGetDeviceInfo(device, CL_DEVICE_NAME, 0, NULL, &valueSize);
  value = (char*)malloc(valueSize);
  clGetDeviceInfo(device, CL_DEVICE_NAME, valueSize, value, NULL);
  printf("Device: %s\n", value);
  free(value);

  // print hardware device version
  clGetDeviceInfo(device, CL_DEVICE_VERSION, 0, NULL, &valueSize);
  value = (char*)malloc(valueSize);
  clGetDeviceInfo(device, CL_DEVICE_VERSION, valueSize, value, NULL);
  printf("Hardware version: %s\n", value);
  free(value);

  // print software driver version
  clGetDeviceInfo(device, CL_DRIVER_VERSION, 0, NULL, &valueSize);
  value = (char*)malloc(valueSize);
  clGetDeviceInfo(device, CL_DRIVER_VERSION, valueSize, value, NULL);
  printf("Software version: %s\n", value);
  free(value);

  // print c version supported by compiler for device
  clGetDeviceInfo(device, CL_DEVICE_OPENCL_C_VERSION, 0, NULL, &valueSize);
  value = (char*)malloc(valueSize);
  clGetDeviceInfo(device, CL_DEVICE_OPENCL_C_VERSION, valueSize, value, NULL);
  printf("OpenCL C version: %s\n", value);
  free(value);

  // print parallel compute units
  clGetDeviceInfo(device, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(maxComputeUnits), &maxComputeUnits, NULL);
  printf("Parallel compute units: %d\n\n", maxComputeUnits);
}

template<typename T>
std::string EasyOpenCL<T>::getErrorString(cl_int err) {

  //From: http://tom.scogland.com/blog/2013/03/29/opencl-errors/

  switch (err){
  case 0: return "CL_SUCCESS";
  case -1: return "CL_DEVICE_NOT_FOUND";
  case -2: return "CL_DEVICE_NOT_AVAILABLE";
  case -3: return "CL_COMPILER_NOT_AVAILABLE";
  case -4: return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
  case -5: return "CL_OUT_OF_RESOURCES";
  case -6: return "CL_OUT_OF_HOST_MEMORY";
  case -7: return "CL_PROFILING_INFO_NOT_AVAILABLE";
  case -8: return "CL_MEM_COPY_OVERLAP";
  case -9: return "CL_IMAGE_FORMAT_MISMATCH";
  case -10: return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
  case -11: return "CL_BUILD_PROGRAM_FAILURE";
  case -12: return "CL_MAP_FAILURE";

  case -30: return "CL_INVALID_VALUE";
  case -31: return "CL_INVALID_DEVICE_TYPE";
  case -32: return "CL_INVALID_PLATFORM";
  case -33: return "CL_INVALID_DEVICE";
  case -34: return "CL_INVALID_CONTEXT";
  case -35: return "CL_INVALID_QUEUE_PROPERTIES";
  case -36: return "CL_INVALID_COMMAND_QUEUE";
  case -37: return "CL_INVALID_HOST_PTR";
  case -38: return "CL_INVALID_MEM_OBJECT";
  case -39: return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
  case -40: return "CL_INVALID_IMAGE_SIZE";
  case -41: return "CL_INVALID_SAMPLER";
  case -42: return "CL_INVALID_BINARY";
  case -43: return "CL_INVALID_BUILD_OPTIONS";
  case -44: return "CL_INVALID_PROGRAM";
  case -45: return "CL_INVALID_PROGRAM_EXECUTABLE";
  case -46: return "CL_INVALID_KERNEL_NAME";
  case -47: return "CL_INVALID_KERNEL_DEFINITION";
  case -48: return "CL_INVALID_KERNEL";
  case -49: return "CL_INVALID_ARG_INDEX";
  case -50: return "CL_INVALID_ARG_VALUE";
  case -51: return "CL_INVALID_ARG_SIZE";
  case -52: return "CL_INVALID_KERNEL_ARGS";
  case -53: return "CL_INVALID_WORK_DIMENSION";
  case -54: return "CL_INVALID_WORK_GROUP_SIZE";
  case -55: return "CL_INVALID_WORK_ITEM_SIZE";
  case -56: return "CL_INVALID_GLOBAL_OFFSET";
  case -57: return "CL_INVALID_EVENT_WAIT_LIST";
  case -58: return "CL_INVALID_EVENT";
  case -59: return "CL_INVALID_OPERATION";
  case -60: return "CL_INVALID_GL_OBJECT";
  case -61: return "CL_INVALID_BUFFER_SIZE";
  case -62: return "CL_INVALID_MIP_LEVEL";
  case -63: return "CL_INVALID_GLOBAL_WORK_SIZE";
  default: return "Unknown OpenCL error";
  }
}

template class EasyOpenCL<char>;
template class EasyOpenCL<int>;
template class EasyOpenCL<float>;

