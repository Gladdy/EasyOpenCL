# Notes 

## `clCreateCommandQueueWithProperties` and `clCreateCommandQueue` 
`clCreateCommandQueue` has been deprecated in favour of `clCreateCommandQueueWithProperties`. However, this happened in the update to OpenCL 2.0. Some devices do not support OpenCL 2.0 yet and they still follow to the older (OpenCL 1.1 or OpenCL 1.2) specification which does not include the depreciation of `clCreateCommandQueue`, hence on older systems you will need to use that. If you experience errors about these functions: missing declarations or being deprecated, try the other function. The arguments are the same.

Should really be fixed by some `#ifdef`s
