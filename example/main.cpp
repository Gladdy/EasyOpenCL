#include "easyopencl.h"

#include <iostream>
#include <exception>
#include <vector>
#include <cstdlib>

int main() {

  try {
    EasyOpenCL framework (SHOW_DEBUG);
    Kernel& simple = framework.loadKernel("simple", "simplekernel.cl");

    simple.setInputBuffer(0, std::vector<int> {1, 2, 3, 4, 5});
    simple.setSingleValue(1, 1000000);

    std::vector<int> v = simple.getValue<int>(1);
    std::cout << v[0] << std::endl;

    //framework.runKernel("simple");
    //framework.showAllValues();

    //framework.cleanup();

    std::cout << "Program finished" << std::endl;
  }
  catch (std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }

}
