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
  Kernel(std::string, cl_context&, cl_device_id*, std::string);
  operator cl_kernel();

  template<typename T>
  void setInputBuffer(uint, std::vector<T>);

  template<typename T>
  void setOutputBuffer(uint);

  template<typename T>
  void setSingleValue(uint, T value);

  template<typename T>
  std::vector<T> getValue(uint);

  void releaseMemObjects();

private:


  std::map<uint, BoundValue> boundValues;
  std::string id;
  cl_kernel kernel;
  cl_context context;
};

#endif
