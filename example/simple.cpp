#include "easyopencl.h"

#include <iostream>
#include <exception>
#include <vector>

int main() {

  try {
    EasyOpenCL<float> framework (NO_DEBUG);

    auto& square = framework.load("squarefloat");
    square.bindInput(0, std::vector<float> { 1.1, 2.2, 3.3, 4.4, 5.5, 11.0 });
    square.bindOutput(1);
    square.evaluate();
    square.showBuffers();
  }
  catch (std::exception& e) { std::cerr << "Error: " << e.what() << std::endl; }

}
