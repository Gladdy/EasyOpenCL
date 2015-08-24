#ifndef _KERNEL_
#define _KERNEL_

#include "errorhandler.h"
#include "boundvalue.h"

#include "opencl-crossplatform.h"

#include <map>
#include <vector>

class Kernel : public ErrorHandler {
  //friend class EasyOpenCL;
public:
  Kernel() {}
  Kernel(std::string, cl_context&, cl_command_queue&
        , cl_device_id*, std::string);
  operator cl_kernel();


  /*******************************************************/
  //  BINDING VALUES TO THE BUFFERS
  /*******************************************************/
  template<typename T>
  void setInputBuffer(uint, std::vector<T>);

  template<typename T>
  void setOutputBuffer(uint, uint);

  template<typename T>
  void setSingleValue(uint, T value);


  /*******************************************************/
  //  RUNNING A KERNEL
  /*******************************************************/
  void runKernel();


  /*******************************************************/
  //  RETRIEVING VALUES FROM THE BUFFERS
  /*******************************************************/
  template<typename T>
  std::vector<T> getValue(uint);


  /*******************************************************/
  //  CLEANING UP
  /*******************************************************/
  void releaseMemObjects();

private:

  std::map<uint, BoundValue> boundValues;
  std::string id;
  size_t vectorSize = 0;

  cl_kernel kernel;
  cl_context context;
  cl_command_queue commandQueue;
};

#endif
