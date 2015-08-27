#include "easyopencl.h"
#include "boundvalue.h"


#include <iostream>
#include <exception>
#include <vector>
#include <cstdlib>

int main() {

  try {
    EasyOpenCL<float> framework (NO_DEBUG);

    auto& square = framework.load("squarefloat");
    auto& cube = framework.load("cubefloat");
    auto& aggregate = framework.load("aggregatefloat");

    std::vector<float> initData {1.0, 5.0, 3.3434324, 2.0, 4.0, 5.0};

    //Specify the entry and exit points for the kernel graph
    square.bindInput(0, initData);
    cube.bindInput(0, initData);
    aggregate.bindOutput(2, 6);

    //Specify the links of the kernel graph
    framework.link(square, aggregate, initData.size(), {{1,0}});
    framework.link(cube, aggregate, initData.size(), {{1,1}});

    // Graphical representation of the structure created above
    //
    // initData      initData
    //   |             |
    //   |             |
    // square (^2)   cube (^3)
    //    \           /
    //     \         /
    //      aggregate (+)
    //          |
    //          |
    //        output

    framework.evaluate("aggregatefloat");
    aggregate.showBuffers();
  }
  catch (std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }

}
