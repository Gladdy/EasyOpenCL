#include "easyopencl.h"

#include <iostream>
#include <exception>
#include <vector>

int main() {

	std::vector<int> in3;

	for (int i = 0; i < 4096; i++) {
		in3.push_back(1);
	}

	try {
		OpenCLFramework<int> framework (SHOW_DEBUG);

		framework.loadKernel("sum.cl");

		framework.addInputBuffer(0, in3);
		framework.addOutputBuffer(1);
		framework.addVectorLength(2);

		framework.runKernel();
		framework.showOutputBuffer();
	}
	catch (std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}

	return 0;
}
