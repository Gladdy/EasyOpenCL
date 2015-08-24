#include "easyopencl.h"

#include <iostream>

#include <sstream>
#include <utility>
#include <algorithm>
#include <cstring>

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


  #ifdef CL_API_SUFFIX__VERSION_2_0
    commandQueue = clCreateCommandQueueWithProperties(context, devices[0], 0, &status);
    checkError("clCreateCommandQueueWithProperties");
  #else
    commandQueue = clCreateCommandQueue(context, devices[0], 0, &status);
    checkError("clCreateCommandQueue");
  #endif
}

template<class T>
Kernel& EasyOpenCL<T>::loadKernel(std::string id, std::string filename) {

  if(kernels.count(id) > 0) {
    raiseError("Identifier '" + id + "' already exists!");
  }

  //Store the kernel in the map
  kernels.emplace(id, Kernel(id, context, devices, filename));
  return kernels[id];
}


/******************************************************************************/
//  EXECUTING THE KERNEL
/******************************************************************************/

/**
 * Run the kernel!
 */
template<class T>
void EasyOpenCL<T>::runKernel(std::string id) {

  if (kernels.count(id) == 0) { raiseError("No kernel by id '" + id +"' exists"); }
  /*
  // Check whether the buffers specified have all been specified
  cl_uint kernelNumArgs;
  status = clGetKernelInfo(kernels.find(id)->second, CL_KERNEL_NUM_ARGS, sizeof(cl_uint), &kernelNumArgs, NULL);

  // Get the kernel max work group size
  size_t maxWorkGroupSize;
  status = clGetKernelWorkGroupInfo(kernel, NULL, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &maxWorkGroupSize, NULL);
  std::cout << maxWorkGroupSize << std::endl;

  std::vector<uint> argumentVector;
  for(auto& kv : values) {
    argumentVector.push_back(kv.first);
  }

  std::sort(argumentVector.begin(), argumentVector.end());

  for(uint i = 0; i < kernelNumArgs; i++) {
    if(i > (argumentVector.size()-1) || argumentVector[i] != i) {
      raiseError("The buffers do not form a sequential set starting at 0: " + std::to_string(i) + " is missing");
    }
  }*/

  // Create a global_work_size array
  // This determines how many workers will execute the kernel
  // If you set a single worker, the process will be sequential
  // If you set too many workers, the OpenCL driver will be unable to function
  size_t global_work_size[] = { vectorSize };
  size_t local_work_size[] = { vectorSize };
  status = clEnqueueNDRangeKernel(commandQueue, kernels.find(id)->second
                                  , 1, NULL, global_work_size, local_work_size
                                  , 0, NULL, NULL);
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

  // for(auto& kv : values) {
  //   std::cout << kv.first << " : ";
  //   showValue(kv.first);
  // }

}


template<typename T>
void EasyOpenCL<T>::cleanup() {

  for (auto& kv : kernels) {

    Kernel& kernel = kv.second;

    status = clReleaseKernel(kernel);
    checkError("clReleaseKernel");

    kernel.releaseMemObjects();
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

template class EasyOpenCL<int>;
//template class EasyOpenCL<char>;
//template class EasyOpenCL<float>;

