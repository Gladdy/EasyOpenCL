## EasyOpenCL - Multikernel branch

### TODO:
* Implementing a sensible way of handling the lengths of vectors. Manually passing the length every time does not make sense, but neither does forcing all vectors to be the same.
* Doing some more template magic for variable argument functions - adding multiple arguments to a kernel in a 'single' function call.
* Automatically generating the local/global work group sizes
* .dot output of your current kernel graph + auto generation of kernel (.cl) file empty function definitions.
* making the kernel calls asynchronous using the clCommandQueue and event pointers
* Template magic for the template classes. Currently if you want to add a new type to operate on you'd have to recompile the library to include it. Maybe just pass a void* and a length and cast it back in the .cl function? If everything would be included in the header files you'd lose the templating madness of explitly defining all used templates, but this comes at the cost of quite a lot of (A) readability and (B) compile speeds.
* Benchmarks of asynchronous vs synchronous kernel calls
* Detect circular dependencies of kernels
* Examples with image processing, deep learning and a renderer/raytracer (tbh the main use case of GPUs)


### Features
* No exposed low-level C, just the C++ STL - Focus on programming the GPU instead of messing about with long C-style OpenCL function calls and manual memory management.
* CMake support for Linux and Mac - No more linking problems if you have installed the correct driver.
* The kernel type is a template: choose from processing `float`s, `int`s or `char`s on your GPU.
* Variable amount of input vectors, output vectors and scalars.
* Human readable OpenCL errors for easy debugging of your program due to the already present error checks on every OpenCL call.
* Includes an example of some slighly more advanced OpenCL to help you get started - computing the sum of a vector in logarithmic time (`example/sum.cl`)
* (WORK IN PROGRESS, check the multikernel branch) The specification of a kernel chain in graph form. It automatically handles interdependencies of kernels and allows you to customize which arguments act as inputs/outputs to which other kernels.

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
