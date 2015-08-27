## EasyOpenCL

### Features
* No exposed low-level C, just the C++ STL - Focus on programming the GPU instead of messing about with long C-style OpenCL function calls and manual memory management.
* CMake support for Linux and Mac - No more linking problems if you have installed the correct driver.
* The kernel type is a template: choose from processing `float`s, `int`s or `char`s on your GPU.
* Variable amount of input vectors, output vectors and scalars.
* Human readable OpenCL errors for easy debugging of your program due to the already present error checks on every OpenCL call.
* Includes an example of some slighly more advanced OpenCL to help you get started - computing the sum of a vector in logarithmic time (`example/sum.cl`)

* Work in progress - a multikernel branch which allows for the specification of a kernel chain in graph form. It automatically handles interdependencies of kernels.
### Overview: it's this easy!
```cpp
// example/main.cpp
try {
  EasyOpenCL<int> framework (SHOW_DEBUG);

  //Load a kernel function
  framework.loadKernel("simplekernel.cl");

  //Bind the inputs and outputs to the kernel function arguments
  framework.setInputBuffer(0, std::vector<int> {1, 2, 3, 4, 5});
  framework.setSingleValue(1, 10);

  //Run the kernel and display the results
  framework.runKernel();
  framework.showAllValues();
}
catch (std::exception& e) {
  std::cerr << "Error: " << e.what() << std::endl;
}
```

```c
// example/simplekernel.cl
__kernel void simplekernel(	__global int* array, const int singlevalue )
{
	int i = get_global_id(0);
	array[i] = array[i] * singlevalue;
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
