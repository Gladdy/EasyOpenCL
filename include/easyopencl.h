#ifndef _EASYOPENCL_
#define _EASYOPENCL_

#ifdef __APPLE__
  #include <OpenCL/cl.h>
  #define clCreateCommandQueueWithProperties clCreateCommandQueue
#else
  #include <CL/cl.h>
#endif

typedef unsigned int uint;

#include <string>
#include <vector>
#include <map>

#include "boundvalue.h"

#define SHOW_DEBUG true
#define NO_DEBUG false

template<class T>
class EasyOpenCL {
public:
	EasyOpenCL(bool);

	// Loading the kernel
	void loadKernel(std::string);

	// Binding the values
	void setInputBuffer(uint,std::vector<T>);
	void setSingleValue(uint, T);
	void setOutputBuffer(uint);

	// Running the kernel
	void runKernel();

	// Retrieving the values
	std::vector<T> getValue(uint);
	void showValue(uint);
	void showAllValues();

	// Cleaning up afterwards
	void cleanup();

private:
	void printDeviceProperty(cl_device_id);

	void raiseError(std::string errorString);
	void checkError(std::string errorLocation);
	std::string getErrorString(cl_int err);

	std::string 			kernelName;
	bool 							info;

	cl_device_id* 		devices;
	cl_int 						status;
	cl_context 				context;
	cl_command_queue 	commandQueue;
	cl_kernel 				kernel;

	size_t vectorSize = -1;
	std::map<uint, BoundValue<T>> values;
};

#endif
