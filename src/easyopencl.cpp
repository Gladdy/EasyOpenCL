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
template<typename T>
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

template<typename T>
Kernel<T>& EasyOpenCL<T>::load(std::string id) {

  if(kernels.count(id)) {
    raiseError("Identifier '" + id + "' already exists!");
  }

  //Store the kernel in the map
  kernels.emplace(id, Kernel<T>(id, context, commandQueue, devices, id + ".cl"));
  return kernels[id];
}

/******************************************************************************/
//  LINKING KERNELS
/******************************************************************************/
template<typename T>
void EasyOpenCL<T>::link( Kernel<T>& source
                        , Kernel<T>& target
                        , uint size
                        , std::map<uint,uint> links) {

  for(auto& kv : links) {
    uint sourceArgPos = kv.first;
    uint targetArgPos = kv.second;

    // Tell the source to generate an output
    source.bindOutput(sourceArgPos, size);

    // Tell the target that an input is promised
    // coming from 'source' , argument position 'sourceArgPos'
    // which should be bound to the targetArgPos
    target.bindPromise(source, sourceArgPos, targetArgPos);
  }

}

/******************************************************************************/
//  EVALUATING
/******************************************************************************/

/**
 * Evaluate a kernel based on the links that went before
 */
template<typename T>
void EasyOpenCL<T>::evaluate(std::string id) {

  if (kernels.count(id) == 0) { raiseError("No kernel by id '" + id +"' exists"); }

  kernels[id].runKernel();
}

/******************************************************************************/
//  EVALUATING
/******************************************************************************/
template<typename T>
void EasyOpenCL<T>::cleanup() {

  for (auto& kv : kernels) {

    Kernel<T>& kernel = kv.second;

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
template class EasyOpenCL<float>;
template class EasyOpenCL<double>;

