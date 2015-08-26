#include "easyopencl.h"
#include "boundvalue.h"


#include <iostream>
#include <exception>
#include <vector>
#include <cstdlib>

int main() {

  try {
    EasyOpenCL<int> framework (SHOW_DEBUG);

    auto& square = framework.load("square");
    auto& aggregate = framework.load("aggregate");

    square.bindInput(0, std::vector<int> {1, 5, 3, 2, 4, 5});
    aggregate.bindInput(0, std::vector<int> {1, 1, 2, 3, 5, 8});
    aggregate.bindOutput(2, 6);

    framework.link(square, aggregate, {{1,1}});

    framework.evaluate("aggregate");

  }
  catch (std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }

}
