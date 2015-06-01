# EasyOpenCL

After installing the proper driver you should be able to `make` the framework.

NOTES:
If your compiler starts to complain about 'clCreateCommandQueueWithProperties'
or 'clCreateCommandQueue' try the other (opencl-framework.cpp line 69).

The reason for this is that 'clCreateCommandQueue' has been deprecated in favour
of 'clCreateCommandQueueWithProperties'. However, this happened in the update to
OpenCL 2.0. Some devices do not support OpenCL 2.0,they still abide to the older
(OpenCL 1.1 or OpenCL 1.2) specification which does not include the depreciation
of 'clCreateCommandQueue', hence on older systems you will need to use that.
