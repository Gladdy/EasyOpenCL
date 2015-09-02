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
  BoundScalar(T val)
  {
    //Defined inline to avoid template hell
    if(std::is_trivial<T>::value == false) {
      raiseError("The scalar type is not trivially copyable");
    }

    size = sizeof(T);
    scalar = new char[size];

    T * scalarT = (T*)scalar;
    *scalarT = val;
  }

  //Move constructor & destructor
  BoundScalar(BoundScalar&&);
  ~BoundScalar();

  template<typename T>
  T getValue() {
    T * scalarT = (T*) scalar;
    return *scalarT;
  }
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

  cl_mem& getMemObject() {
    return buffer;
  }

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
  friend class Kernel<T>;
public:
  //Main constructor
  BoundPromise(Kernel<T>*, uint, uint);

  //Move constructor & destructor
  BoundPromise(BoundPromise&&);
  ~BoundPromise();

  uint getSize();

private:

  Kernel<T> * sourceKernel;
  uint sourceArgPos;
  uint targetArgPos;
  uint size;
};

#endif
