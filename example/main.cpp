#include "easyopencl.h"
#include "boundvalue.h"


#include <iostream>
#include <exception>
#include <vector>
#include <cstdlib>

struct orientation {
  float x;
  float y;
  float z;
};

int main() {

  try {
    EasyOpenCL<float> framework (false);

    auto& generate = framework.load("generatefloat");
    auto& square = framework.load("squarefloat");
    auto& cube = framework.load("cubefloat");
    auto& aggregate = framework.load("aggregatefloat");

    std::vector<float> initData {1.0342, 5.00001, 3.3434324, 2234.0, 423432.0, 5234324.0};

    //Specify the entry and exit points for the kernel graph
    cube.bindInput(0, initData);  // (a)
    aggregate.bindOutput(2, 6);   // (b)

    //Specify the links of the kernel graph
    framework.link(generate, square, initData.size(), {{0,0}});   // (1)
    framework.link(square, aggregate, initData.size(), {{1,0}});  // (2)
    framework.link(cube, aggregate, initData.size(), {{1,1}});    // (3)

    // Graphical representation of the structure created above
    //
    // generate      initData
    //   |             |
    //   |(1)          |(a)
    // square (^2)   cube (^3)
    //    \           /
    //     \(2)   (3)/
    //      aggregate (+)
    //          |(b)
    //          |
    //        output

    framework.evaluate("aggregatefloat");
    aggregate.showBuffers();
  }
  catch (std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }

}
