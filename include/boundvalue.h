#ifndef _BOUNDVALUE_
#define _BOUNDVALUE_

#ifdef __APPLE__
  #include <OpenCL/cl.h>
#else
  #include <CL/cl.h>
#endif

enum BoundValueType { CL_MEM, SCALAR };

template<class T>
class BoundValue {
public:
  BoundValue() {}
  BoundValue(cl_mem);
  BoundValue(T);
  bool isScalar();
  T getScalarValue();

  operator cl_mem();

private:
  BoundValueType type;

  cl_mem cl_mem_value;
  T scalar_value;
};



#endif
