#ifndef _KERNEL_
#define _KERNEL_

#include "errorhandler.h"
#include "boundvalue.h"

#include "opencl-crossplatform.h"

#include <map>
#include <vector>

template<typename T>
class Kernel : public ErrorHandler {

  template <typename> friend class EasyOpenCL;

public:
  Kernel() {}
  Kernel(std::string
        , cl_context&
        , cl_command_queue&
        , cl_device_id*
        , std::string);
  operator cl_kernel();


  /*******************************************************/
  //  BINDING VALUES TO THE BUFFERS
  /*******************************************************/
  void bindInput(uint, std::vector<T>);
  void bindOutput(uint, uint);

  template<typename S>
  void bindScalar(uint, S value);

  void bindPromise(uint);

  /*******************************************************/
  //  RUNNING A KERNEL
  /*******************************************************/
  void runKernel();


  /*******************************************************/
  //  RETRIEVING VALUES FROM THE BUFFERS
  /*******************************************************/
  std::vector<T> getBuffer(uint);
  void showBuffer(uint);
  void showBuffers();

  /*******************************************************/
  //  CLEANING UP
  /*******************************************************/
  void releaseMemObjects();

private:

  /*******************************************************/
  //  CONTROLLING THE BOUNDVALUE MAPS
  /*******************************************************/
  void erase(uint);
  void putBoundScalar(uint, const BoundScalar&&);
  void putBoundBuffer(uint, const BoundBuffer&&);
  void putBoundPromise(uint, const BoundPromise<T>&&);
  std::map<uint, BoundScalar> boundScalars;
  std::map<uint, BoundBuffer> boundBuffers;
  std::map<uint, BoundPromise<T>> boundPromises;

  std::string id;
  size_t vectorSize = 5;

  cl_kernel kernel;
  cl_context context;
  cl_command_queue commandQueue;
};

#endif
