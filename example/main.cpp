#include "easyopencl.h"

#include <iostream>
#include <exception>
#include <vector>
#include <cstdlib>

int main() {

  try {
    EasyOpenCL<int> framework (SHOW_DEBUG);
    framework.loadKernel("simplekernel.cl");

    framework.setInputBuffer(0, std::vector<int> {1, 2, 3, 4, 5});
    framework.setSingleValue(1, 10);
    framework.setOutputBuffer(2);

    framework.runKernel();
    framework.showAllValues();
  }
  catch (std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
}
