## EasyOpenCL

> The easiest way to get started with OpenCL!

### Overview : it's this simple
```cpp

std::vector<int> input_1 (5,5);
std::vector<int> input_2 { 1, 2, 3, 4, 5 };

try {
		EasyOpenCL<int> framework (SHOW_DEBUG);

		framework.loadKernel("arithmetic.cl");

		framework.addInputBuffer(0, input_1);
		framework.addInputBuffer(1, input_2);
		framework.addOutputBuffer(3);
		
		framework.runKernel();
		
		framework.showOutputBuffer();
	}
	catch (std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}

```

### Getting started
```
git clone https://github.com/Gladdy/EasyOpenCL.git
cd EasyOpenCL
mkdir build && cd build
cmake ..
make 
./test
```

