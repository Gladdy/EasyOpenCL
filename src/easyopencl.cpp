#include "easyopencl.h"

#include <iostream>
#include <exception>
#include <stdexcept>
#include <fstream>
#include <sstream>

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

template<class T>
EasyOpenCL<T>::EasyOpenCL(bool printData) {

	info = printData;
	cl_uint numPlatforms;	//the NO. of platforms
	cl_platform_id platform = NULL;	//the chosen platform
	status = clGetPlatformIDs(0, NULL, &numPlatforms);
	checkError("clGetPlatformIDs");

	//Just take the first platform available
	if (numPlatforms > 0)
	{
		cl_platform_id* platforms = (cl_platform_id*)malloc(numPlatforms* sizeof(cl_platform_id));
		status = clGetPlatformIDs(numPlatforms, platforms, NULL);
		platform = platforms[0];
		free(platforms);
	}

	//Try to get the GPU, if not available, take the CPU
	cl_uint	numDevices = 0;
	status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 0, NULL, &numDevices);
	checkError("clGetDeviceIDs");

	if (numDevices == 0)	//no GPU available.
	{
		std::cout << "No GPU device available." << std::endl;
		std::cout << "Choose CPU as default device." << std::endl;
		status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 0, NULL, &numDevices);
		devices = (cl_device_id*)malloc(numDevices * sizeof(cl_device_id));
		status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, numDevices, devices, NULL);
	}
	else
	{
		//Pick the GPU
		devices = (cl_device_id*)malloc(numDevices * sizeof(cl_device_id));
		status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, numDevices, devices, NULL);
	}

	//Print the data about the picked device
	if (info) {
		printDeviceProperty(*devices);
	}

	//Create an OpenCL context and a command queue
	context = clCreateContext(NULL, 1, devices, NULL, NULL, &status);
	checkError("clCreateContext");

	commandQueue = clCreateCommandQueue(context, devices[0], 0, &status);
	checkError("clCreateCommandQueueWithProperties");
}

template<class T>
void EasyOpenCL<T>::loadKernel(std::string filename) {

	std::ifstream f(filename);
	if (!f.good()) {
		raiseError("Unable to open kernel file: " + filename);
	}

	std::stringstream buffer;
	buffer << f.rdbuf();

	std::string fileContents = buffer.str();

	if (info) {
		std::cout << "Kernel content:\n " << fileContents << std::endl;
	}

	const char *source = fileContents.c_str();
	size_t sourceSize = fileContents.length();
	program = clCreateProgramWithSource(context, 1, &source, &sourceSize, &status);
	checkError("clCreateProgramWithSource");

	status = clBuildProgram(program, 1, devices, NULL, NULL, NULL);
	checkError("clBuildProgram");

	kernelName = filename.substr(0, filename.find('.'));
	kernel = clCreateKernel(program, kernelName.c_str(), &status);
	checkError("clCreateKernel");
}

template<typename T>
void EasyOpenCL<T>::addInputBuffer(int argumentCounter, std::vector<T> input) {

	if (vectorSize != -1 && vectorSize != input.size())  {
		raiseError("You passed vectors with different lengths to the framework");
	}
	vectorSize = input.size();

	cl_mem inputBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, vectorSize * sizeof(T), (void *)&input[0], NULL);
	status = clSetKernelArg(kernel, argumentCounter, sizeof(cl_mem), (void *)&inputBuffer);
	checkError("clSetKernelArg input");
	inputBufferVector.push_back(inputBuffer);
}

template<typename T>
void EasyOpenCL<T>::addOutputBuffer(int argumentCounter) {

	if (vectorSize == -1)  {
		raiseError("Please pass the input buffer first");
	}

	if (outputBufferSet == true) {
		raiseError("You have already set an output buffer");
	}

	outputBuffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, vectorSize * sizeof(T), NULL, NULL);
	status = clSetKernelArg(kernel, argumentCounter, sizeof(cl_mem), (void *)&outputBuffer);
	checkError("clSetKernelArg output");
	outputBufferSet = true;
}

template<class T>
void EasyOpenCL<T>::runKernel() {
	size_t global_work_size[1] = { vectorSize };
	status = clEnqueueNDRangeKernel(commandQueue, kernel, 1, NULL, global_work_size, NULL, 0, NULL, NULL);
	checkError("clEnqueueNDRangeKernel");
}

template<typename T>
std::vector<T> EasyOpenCL<T>::getOutputBuffer() {

	T* hostOutputBuffer = new T[vectorSize];
	status = clEnqueueReadBuffer(commandQueue, outputBuffer, CL_TRUE, 0, vectorSize * sizeof(T), hostOutputBuffer, 0, NULL, NULL);
	checkError("clEnqueueReadBuffer");

	std::vector<T> hostOutputVector;
	hostOutputVector.reserve(vectorSize);
	for (unsigned i = 0; i < vectorSize; i++) {
		hostOutputVector.push_back(hostOutputBuffer[i]);
	}

	return hostOutputVector;
}

template<typename T>
void EasyOpenCL<T>::showOutputBuffer() {

	std::vector<T> output = getOutputBuffer();

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
void EasyOpenCL<T>::cleanup() {
	status = clReleaseKernel(kernel);
	checkError("clReleaseKernel");
	status = clReleaseProgram(program);
	checkError("clReleaseProgram");

	for (cl_mem inputBuffer : inputBufferVector) {
		status = clReleaseMemObject(inputBuffer);
		checkError("clReleaseMemObject");
	}

	status = clReleaseMemObject(outputBuffer);
	checkError("clReleaseMemObject");
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
