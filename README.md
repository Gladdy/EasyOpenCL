## EasyOpenCL

### Features:
* A Visual Studio example project with guide on how to specify the include and library folders
* CMake support for Linux and Mac
* No exposed OpenCL or manual memory management calls, just the C++ STL.
* Parametrized kernel types, choose from processing `float`s, `int`s or `char`s on your GPU.
* Variable amount of input vectors
* Human readable OpenCL errors for easy debugging of your program.

### Overview: it's this easy!
```cpp
std::vector<int> input_1 (5,5);
std::vector<int> input_2 { 1, 2, 3, 4, 5 };

try {
    EasyOpenCL<int> framework (SHOW_DEBUG);

    framework.loadKernel("arithmetic.cl");
    framework.addInputBuffer(0, input_1);
    framework.addInputBuffer(1, input_2);
    framework.addOutputBuffer(2);
	
    framework.runKernel();
	
    framework.showOutputBuffer();
}
catch (std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
}
```

### Getting started
###### Windows
* Clone or download this repository
* Open the example project solution (`example/VC++2013/EasyOpenCL.sln`) in Visual Studio
* Specify the proper include and library directories using the guide in the README.md
* Build and run the example using `Ctrl-F5`

###### Linux and Mac
```
git clone https://github.com/Gladdy/EasyOpenCL.git
cd EasyOpenCL
mkdir build && cd build
cmake ..
make 
./test
```

### Thanks to:
* Dhruba Bandopadhyay: [OpenCL Cookbook: Listing all devices and their critical attributes](http://dhruba.name/2012/08/14/opencl-cookbook-listing-all-devices-and-their-critical-attributes/)
* elhigu: [cmake-findopencl](https://github.com/elhigu/cmake-findopencl)
* Tom Scogland: [OpenCL error checking](http://tom.scogland.com/blog/2013/03/29/opencl-errors/)
