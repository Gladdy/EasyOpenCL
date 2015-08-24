#ifndef _BOUNDVALUE_
#define _BOUNDVALUE_

#include "opencl-crossplatform.h"
#include "errorhandler.h"

enum BoundValueType { CL_MEM, SCALAR };

class BoundValue : public ErrorHandler {
public:

  /*******************************************************/
  //  CONSTRUCTORS & DESTRUCTOR
  /*******************************************************/
  //BoundValue();
  BoundValue(cl_mem, uint);

  template<typename T>
  BoundValue(T);

  //BoundValue(const BoundValue&);    //copy
  BoundValue(BoundValue&&);         //move

  ~BoundValue();

  /*******************************************************/
  //  SCALAR OPERATIONS
  /*******************************************************/
  bool isScalar();

  template<typename T>
  T getScalarValue();

  /*******************************************************/
  //  VECTOR (CL_MEM) OPERATIONS
  /*******************************************************/
  uint getVectorSize();
  operator cl_mem();

private:
  const BoundValueType type = SCALAR;

  // Vector members
  uint vectorSize = 0;
  cl_mem cl_mem_value;

  // Scalar members
  size_t scalarSize = 0;
  char * scalarValue;

  const bool debug = false;
};



#endif
