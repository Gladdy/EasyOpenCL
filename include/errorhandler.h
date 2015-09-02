#ifndef _ERRORHANDLER_
#define _ERRORHANDLER_

#include "opencl-crossplatform.h"

#include <string>

class ErrorHandler {
public:
  void raiseError(std::string errorString);
  void checkError(std::string errorLocation);
  std::string getErrorString(cl_int err);
  cl_int status;
};

#endif
