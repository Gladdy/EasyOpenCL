#include "easyopencl.h"

#include <iostream>
#include <exception>
#include <vector>
#include <cstdlib>

int main() {

	std::vector<float> input;

	for(int i = 0; i < 8; i++) {
		float f = static_cast <float> (rand()) / RAND_MAX;
		input.push_back(f);
	}

	try {
		EasyOpenCL<float> framework (SHOW_DEBUG);
		framework.loadKernel("arithmetic.cl");

		framework.setInputBuffer(0, input);
		framework.setInputBuffer(1, input);
		framework.setOutputBuffer(2);

		framework.runKernel();
		framework.showAllValues();
	}
	catch (std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}

	return 0;
}
