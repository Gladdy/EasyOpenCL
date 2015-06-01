## Notes 

#### `clCreateCommandQueueWithProperties` and `clCreateCommandQueue` 
`clCreateCommandQueue` has been deprecated in favour of `clCreateCommandQueueWithProperties` in the OpenCL 2.0 specification. Some devices do not support OpenCL 2.0 yet and they still follow to the older (OpenCL 1.1 or OpenCL 1.2) specification which does not include the depreciation of `clCreateCommandQueue`. If you experience errors about these functions: missing declarations or being deprecated, try the other function. The arguments are the same.

Should really be fixed by some `#ifdef`s
