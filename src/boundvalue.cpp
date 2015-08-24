#include "boundvalue.h"

#include <cstring>
#include <iostream>

/*******************************************************/
//  CONSTRUCTORS & DESTRUCTOR
/*******************************************************/
// BoundValue::BoundValue() {
//   std::cout << "default constructor" << std::endl;

// }
BoundValue::BoundValue(cl_mem val, uint vectorSize_) : type(CL_MEM) {
  if(debug)
    std::cout << "cl_mem constructor" << std::endl;

  vectorSize = vectorSize_;
  cl_mem_value = val;
}

template<typename T>
BoundValue::BoundValue(T val) : type(SCALAR) {

  if(debug)
    std::cout << "custom scalar constructor" << std::endl;

  scalarSize = sizeof(T);
  scalarValue = new char[scalarSize];

  //std::memcpy(scalarValue, &val, scalarSize);

  T * location = (T*)scalarValue;
  *location = val;

  std::cout << "location: " << location << "\t" << *location << std::endl;

  for(int i = 0; i < scalarSize; i++) {
    std::cout << "scalarValue " << i << "\t" << scalarValue
              << "\t" << scalarValue[i] << std::endl;
  }
}

BoundValue::~BoundValue() {
  if(debug)
    std::cout << "Destroying object" << std::endl;

  if(type == SCALAR) {
    delete scalarValue;
  }
}

// BoundValue::BoundValue(const BoundValue& bv) : type(bv.type) {
//   if(debug)
//     std::cout << "Copy constructor called" << std::endl;

//   if(type == CL_MEM) {
//     vectorSize = bv.vectorSize;
//     cl_mem_value = bv.cl_mem_value;
//   } else {
//     scalarSize = bv.scalarSize;
//     std::memcpy(scalarValue, &bv.scalarValue, scalarSize);
//   }
// }

BoundValue::BoundValue(BoundValue&& bv) : type(bv.type) {
  if(debug)
    std::cout << "Move constructor called" << std::endl;

  if(type == CL_MEM) {
    vectorSize = bv.vectorSize;
    cl_mem_value = bv.cl_mem_value;
  } else {
    scalarSize = bv.scalarSize;
    std::cout << "attempting move" << std::endl;

    scalarValue = new char[scalarSize];

    std::memcpy(scalarValue, &bv.scalarValue, scalarSize);
    std::cout << "moved" << std::endl;
  }
}

/*******************************************************/
//  SCALAR OPERATIONS
/*******************************************************/
bool BoundValue::isScalar() {
  return (type == SCALAR);
}

template<typename T>
T BoundValue::getScalarValue() {

  if(scalarValue == nullptr) {
    raiseError("Scalar value has not been specified");
  }

  for(int i = 0; i < scalarSize; i++) {
    std::cout << scalarValue[i] << "\t";
  }
  std::cout << "attempting to return the value" << std::endl;


  return (T)*scalarValue;
}

/*******************************************************/
//  VECTOR (CL_MEM) OPERATIONS
/*******************************************************/
BoundValue::operator cl_mem() {
  return cl_mem_value;
}

uint BoundValue::getVectorSize() {
  return vectorSize;
}

template BoundValue::BoundValue(int);
template int BoundValue::getScalarValue<int>();

