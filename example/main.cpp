#include "easyopencl.h"

#include <iostream>
#include <exception>
#include <vector>
#include <cstdlib>

int main() {

  try {
    EasyOpenCL<int> framework (SHOW_DEBUG);
    Kernel k = framework.loadKernel("simple", "simplekernel.cl");

    framework.setInputBuffer("simple", 0, std::vector<int> {1, 2, 3, 4, 5});
    framework.setSingleValue("simple", 1, 10);

    framework.runKernel("simple");
    framework.showAllValues();
  }
  catch (std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
}
