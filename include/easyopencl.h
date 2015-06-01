#ifndef _OPENCLFRAMEWORK_
#define _OPENCLFRAMEWORK_

#include <CL/cl.h>

#include <string>
#include <vector>

#define SHOW_DEBUG true
#define NO_DEBUG false

template<class T>
class OpenCLFramework {
public:
	OpenCLFramework(bool);
	void loadKernel(std::string);

	void addInputBuffer(int,std::vector<T>);
	void addOutputBuffer(int);
	void runKernel();
	std::vector<T> getOutputBuffer();
	void showOutputBuffer();
	void cleanup();

private:
	void printDeviceProperty(cl_device_id);

	void raiseError(std::string errorString);
	void checkError(std::string errorLocation);
	std::string getErrorString(cl_int err);

	size_t vectorSize = -1;
	std::string kernelName;

	bool info;
	cl_device_id* devices;
	cl_int	status;
	cl_context context;
	cl_command_queue commandQueue;
	cl_program program;
	cl_kernel kernel;


	std::vector<cl_mem> inputBufferVector;
	cl_mem outputBuffer;
	bool outputBufferSet = false;
};

#endif
