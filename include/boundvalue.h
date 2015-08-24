#ifndef _BOUNDVALUE_
#define _BOUNDVALUE_

#include "opencl-crossplatform.h"
#include "errorhandler.h"

enum BoundValueType { CL_MEM, SCALAR };

class BoundValue : public ErrorHandler {
public:
  BoundValue() {}
  BoundValue(cl_mem);
  ~BoundValue();

  template<typename T>
  BoundValue(T);

  bool isScalar();

  template<typename T>
  T getScalarValue();

  operator cl_mem();

private:
  BoundValueType type;

  cl_mem cl_mem_value;

  char * scalarValue = nullptr;
};



#endif
