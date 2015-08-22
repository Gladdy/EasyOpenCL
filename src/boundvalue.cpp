#include "boundvalue.h"

template<class T>
BoundValue<T>::BoundValue(cl_mem val) {
  cl_mem_value = val;
  type = CL_MEM;
}

template<class T>
BoundValue<T>::BoundValue(T val) {
  scalar_value = val;
  type = SCALAR;
}

template<class T>
bool BoundValue<T>::isScalar() {
  return (type == SCALAR);
}

template<class T>
T BoundValue<T>::getScalarValue() {
  return scalar_value;
}

template<class T>
BoundValue<T>::operator cl_mem() {
  return cl_mem_value;
}



template class BoundValue<char>;
template class BoundValue<int>;
template class BoundValue<float>;
