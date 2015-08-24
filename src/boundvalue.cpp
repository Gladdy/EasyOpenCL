#include "boundvalue.h"

#include <cstring>

BoundValue::BoundValue(cl_mem val) {
  cl_mem_value = val;
  type = CL_MEM;
}

template<typename T>
BoundValue::BoundValue(T val) {

  type = SCALAR;

  if(scalarValue != nullptr) {
    delete scalarValue;
  }

  scalarValue = new char[sizeof(T)];
  std::memcpy(scalarValue, &val, sizeof(T));
}

BoundValue::~BoundValue() {
  if(scalarValue != nullptr) {
    delete scalarValue;
  }
}


bool BoundValue::isScalar() {
  return (type == SCALAR);
}

template<typename T>
T BoundValue::getScalarValue() {

  if(scalarValue == nullptr) {
    raiseError("Scalar value has not been specified");
  }

  return (T)*scalarValue;
}

BoundValue::operator cl_mem() {
  return cl_mem_value;
}

template BoundValue::BoundValue(int);
template int BoundValue::getScalarValue<int>();

