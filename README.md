## EasyOpenCL

### Features
* No exposed low-level C, just the C++ STL - Focus on programming the GPU instead of messing about with long C-style OpenCL function calls and manual memory management.
* CMake support for Linux and Mac - No more linking problems if you have installed the correct driver.
* Template kernel processing type: choose from processing `float`s, `int`s or `char`s on your GPU.
* Variable amount of input and output vectors of arbitrary length.
* Support for scalar values: pass additional structs to your kernel, eg. transformation matrices or custom constants.
* Chain kernels together in order to create a true pipeline on your GPU in which kernels can depend on multiple others.
* Human readable OpenCL errors for easy debugging and teaching of the OpenCL basics.
* Includes an example of some slighly more advanced OpenCL to help you get started - computing the sum of a vector in logarithmic time (`example/sum.cl`)

### Overview: it's this easy!
```cpp
// example/simple.cpp
try {
  EasyOpenCL<float> framework (NO_DEBUG);
  
  //Register the kernel with the framework
  auto& square = framework.load("squarefloat");
  
  //Bind the input buffer (initial values from a std::vector) and the output buffer
  square.bindInput(0, std::vector<float> { 1.1, 2.2, 3.3, 4.4, 5.5, 11.0 });
  square.bindOutput(1);
  
  //Evaluate the kernel
  square.evaluate();
  
  //Display the results
  square.showBuffer(1);
  // [ 1.21, 4.84, 10.89, 19.36, 30.25, 121 ]
} catch (std::exception& e) { std::cerr << "Error: " << e.what() << std::endl; }
```
```c
// kernels/squarefloat.cl
__kernel void squarefloat(__global float* input, __global float* output)
{
  int i = get_global_id(0);
  output[i] = input[i] * input[i];
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
./simple
```

### TODO:
* Automatic generation of local and global work group sizes
* .dot output of your current kernel graph 
* Automatic generation of stub kernel (.cl) files based on the kernel links specified.
* Asynchronous kernel calls
* Benchmarks of asynchronous vs synchronous kernel calls
* Detect circular dependencies of kernels
* More examples - image processing, deep learning and a renderer/raytracer

### Thanks to:
* Dhruba Bandopadhyay: [OpenCL Cookbook: Listing all devices and their critical attributes](http://dhruba.name/2012/08/14/opencl-cookbook-listing-all-devices-and-their-critical-attributes/)
* elhigu: [cmake-findopencl](https://github.com/elhigu/cmake-findopencl)
* Tom Scogland: [OpenCL error checking](http://tom.scogland.com/blog/2013/03/29/opencl-errors/)
