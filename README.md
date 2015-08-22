## EasyOpenCL

### Features:
* CMake support for Linux and Mac
* No exposed OpenCL or manual memory management calls, just the C++ STL.
* Parametrized kernel types, choose from processing `float`s, `int`s or `char`s on your GPU.
* Variable amount of input vectors
* Human readable OpenCL errors for easy debugging of your program.

### Overview: it's this easy!
```cpp
try {
  EasyOpenCL<int> framework (SHOW_DEBUG);

  //Load a kernel function
  framework.loadKernel("simplekernel.cl");

  //Bind the inputs and outputs to the kernel function arguments
  framework.setInputBuffer(0, std::vector<int> {1, 2, 3, 4, 5});
  framework.setSingleValue(1, 10);
  framework.setOutputBuffer(2);

  //Run the kernel and display the results
  framework.runKernel();
  framework.showAllValues();
}
catch (std::exception& e) {
  std::cerr << "Error: " << e.what() << std::endl;
}
```

```c
__kernel void simplekernel( __global int* input, const int singlevalue
                          , __global int* output ) {
  int i = get_global_id(0);
  output[i] = input[i] * singlevalue;
}
```

### Getting started
All platforms:
* Update your graphics drivers
* Install the drivers with OpenCL support (NVIDIA CUDA Toolkit or AMD APP SDK)


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
