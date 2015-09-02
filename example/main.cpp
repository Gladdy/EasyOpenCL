#include "easyopencl.h"
#include "mac.clh"

#include <iostream>
#include <exception>
#include <vector>
#include <cstdlib>

int main() {

  MAC macdata {3.0, 17.0};
  std::vector<float> initData {1.0342, 5.00001, 3.3434324, 2234.0, 423432.0, 5.0};

  try {
    EasyOpenCL<float> framework (false);

    // input:   -
    // output:  the index as floating point value
    auto& generate = framework.load("generatefloat");

    // input:   float vector x
    // output:  x.^2
    auto& square = framework.load("squarefloat");

    // input:   float vector x
    //          MAC { float mult; float add; }
    // output:  x.*mult + add
    auto& mac = framework.load("macfloat");

    // input:   float vector x
    //          float vector y
    // output:  x + y
    auto& aggregate = framework.load("aggregatefloat");


    //Specify the entry and exit points for the kernel graph
    mac.bindInput(0, initData);   // (a)
    mac.bindScalar<MAC>(2, macdata);  // (b)
    aggregate.bindOutput(2);      // (c)

    //Specify the links of the kernel graph
    framework.link(generate, square, {{0,0}});   // (1)
    framework.link(square, aggregate, {{1,0}});  // (2)
    framework.link(mac, aggregate, {{1,1}});    // (3)

    // Graphical representation of the structure created above
    //
    // generate       initData
    //   |               |
    //   |(1)            |(a)
    //   |               |    (b)
    // square (^2)      mac ------ (*mult + add) mult = 3.0
    //    \             /                        add = 17.0
    //     \(2)        /(3)
    //      \         /
    //       aggregate (+)
    //           |
    //           |(c)
    //           |
    //         output

    framework.evaluate("aggregatefloat");
    aggregate.showBuffers();
  }
  catch (std::exception& e) { std::cerr << "Error: " << e.what() << std::endl; }

}
