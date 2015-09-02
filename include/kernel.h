#ifndef _KERNEL_
#define _KERNEL_

#include "easyopencl.h"
#include "errorhandler.h"
#include "boundvalue.h"


#include "opencl-crossplatform.h"

#include <map>
#include <vector>

template <typename> class EasyOpenCL;

template<typename T>
class Kernel : public ErrorHandler {

  template <typename> friend class EasyOpenCL;

public:

  Kernel() {}
  /*******************************************************/
  //  BINDING VALUES TO THE BUFFERS
  /*******************************************************/
  void bindInput(uint, std::vector<T>);

  void bindOutput(uint);
  void bindOutput(uint, uint);

  template<typename S>
  void bindScalar(uint argPos, S value) {
    //Inline definition to avoid recompilation of the entire library
    //if you just want to add a new scalar type.
    status = clSetKernelArg(kernel, argPos, sizeof(S), &value);
    checkError("clSetKernelArg singleValue " + std::to_string(argPos));
    erase(argPos);
    boundScalars.emplace(argPos, BoundScalar(value));
  }

  void bindPromise(Kernel<T>&, uint, uint);

  /*******************************************************/
  //  RUNNING A KERNEL
  /*******************************************************/
  void evaluate();


  /*******************************************************/
  //  RETRIEVING VALUES FROM THE BUFFERS
  /*******************************************************/
  std::vector<T> getBuffer(uint);
  void showBuffer(uint);
  void showBuffers();

  /*******************************************************/
  //  CLEANING UP
  /*******************************************************/
  void releaseMemObjects();

  /*******************************************************/
  //  UTILITY
  /*******************************************************/
  std::string getId() { return id; }
  uint getExecutionCount() { return executionCounter; }

private:

  Kernel(std::string, cl_context&, cl_command_queue&
        , cl_device_id*, std::string, EasyOpenCL<T>* );
  operator cl_kernel();

  /*******************************************************/
  //  CONTROLLING THE BOUNDVALUE MAPS
  /*******************************************************/
  void erase(uint);
  std::map<uint, BoundScalar> boundScalars;
  std::map<uint, BoundBuffer> boundBuffers;
  std::map<uint, BoundPromise<T>> boundPromises;

  std::string id;
  size_t vectorSize = -1;

  cl_kernel kernel;
  cl_context context;
  cl_command_queue commandQueue;

  uint executionCounter = 0;
  const bool debug = false;
  EasyOpenCL<T> * framework;
};

#endif
