#include "easyopencl.h"

#include <iostream>
#include <exception>
#include <vector>

int main() {

	std::vector<int> in1(5, 5);
	std::vector<int> in2{ 123, 213, 121, 1231, 123 };
	std::vector<int> in3;

	for (int i = 0; i < 5; i++) {
		in3.push_back(i); // {0, 1, 2, 3, 4, 5}
	}

	try {
		OpenCLFramework<int> framework (false);

		/*
		DO NOT CALL YOUR FILE KERNEL.CL
		The file name (minus extension) should be the same as the function name in the file.
		If you call it "kernel.cl", the compiler gets confused as "kernel" is a reserved keyword in OpenCL-C.
		*/
		framework.loadKernel("arithmetic.cl");

		framework.addInputBuffer(0, in1);
		framework.addInputBuffer(1, in2);
		framework.addInputBuffer(2, in3);
		framework.addOutputBuffer(3);

		framework.runKernel();
		framework.showOutputBuffer();
	
	
		for (int i = 0; i < 1000; i++) {
			framework.addInputBuffer(i%3, in2);
			framework.addInputBuffer((i+1)%3, in3);
			framework.runKernel();
			//framework.showOutputBuffer();
		}
	
	}
	catch (std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}

	return 0;
}
