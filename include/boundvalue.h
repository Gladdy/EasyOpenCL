#ifndef _BOUNDVALUE_
#define _BOUNDVALUE_

#include "errorhandler.h"

#include "opencl-crossplatform.h"

#include <memory>

class BoundValue : public ErrorHandler {

};



/*******************************************************/
//  Constant scalars
/*******************************************************/
class BoundScalar : public BoundValue {
public:
  //Main constructor
  template<typename T>
  BoundScalar(T);

  //Move constructor & destructor
  BoundScalar(BoundScalar&&);
  ~BoundScalar();

  template<typename T>
  T getValue();
private:
  size_t size = 0;
  char * scalar;
};


/*******************************************************/
//  Buffers
/*******************************************************/
class BoundBuffer : public BoundValue {
public:
  //Main constructor
  BoundBuffer(cl_mem, uint);

  //Move constructor & destructor
  BoundBuffer(BoundBuffer&&);
  ~BoundBuffer();

  uint getSize();
  operator cl_mem();
private:
  uint size = 0;
  cl_mem buffer;
};

/*******************************************************/
//  Promises for buffers
/*******************************************************/
template<typename> class Kernel;

template<typename T>
class BoundPromise : public BoundValue {
public:
  //Main constructor
  BoundPromise(Kernel<T>*);

  //Move constructor & destructor
  BoundPromise(BoundPromise&&);
  ~BoundPromise();
private:

  Kernel<T> * sourceKernel;
};

#endif
