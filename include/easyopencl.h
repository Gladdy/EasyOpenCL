#ifndef _EASYOPENCL_
#define _EASYOPENCL_

#include "boundvalue.h"
#include "errorhandler.h"
#include "kernel.h"

#include "opencl-crossplatform.h"

#include <string>
#include <vector>
#include <map>

#define SHOW_DEBUG true
#define NO_DEBUG false

class EasyOpenCL : public ErrorHandler {
	//friend class Kernel;
public:
	EasyOpenCL(bool);

	// Loading the kernel
	Kernel& loadKernel(std::string, std::string);

	// Binding the values
	//void setInputBuffer(std::string, uint,std::vector<T>);
	//void setSingleValue(std::string, uint, T);
	//void setOutputBuffer(std::string, uint);

	// Running the kernel
	void runKernel(std::string id);

	// Retrieving the values
	//std::vector<T> getValue(uint);
	void showValue(uint);
	void showAllValues();

	// Cleaning up afterwards
	void cleanup();

private:
	void printDeviceProperty(cl_device_id);

	bool 							info;

	cl_device_id* 		devices;
	cl_context 				context;
	cl_command_queue 	commandQueue;

	size_t vectorSize = -1;
	std::map<std::string, Kernel> kernels;
};

#endif
