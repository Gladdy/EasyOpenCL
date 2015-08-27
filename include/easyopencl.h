#ifndef _EASYOPENCL_
#define _EASYOPENCL_


#include "errorhandler.h"
#include "boundvalue.h"
#include "kernel.h"

#include "opencl-crossplatform.h"

#include <string>
#include <vector>
#include <map>

#define SHOW_DEBUG true
#define NO_DEBUG false

template<typename T>
class EasyOpenCL : public ErrorHandler {
public:
	EasyOpenCL(bool);

	// Loading a kernel
	Kernel<T>& load(std::string);

	// Linking the buffers
	void link(Kernel<T>&, Kernel<T>&, uint, std::map<uint,uint>);

	// Evaluating the results
	void evaluate(std::string id);

	// Cleaning up afterwards
	void cleanup();

private:
	void printDeviceProperty(cl_device_id);

	bool 							info;

	cl_device_id* 		devices;
	cl_context 				context;
	cl_command_queue 	commandQueue;

	std::map<std::string, Kernel<T>> kernels;
};

#endif
