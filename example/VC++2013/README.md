## Visual Studio example project - A step by step guide
You will need so set up the project before you can build and run this example. The confinguration still needs the include location for the OpenCL headers and the corresponding implementation library.

### Installing Visual Studio
Visual Studio 2015 Community is available for free from [the Microsoft Visual Studio website](https://www.visualstudio.com/). Any version starting from 2013 (used to create this example project) should work.

### Downloading the proper graphics drivers for OpenCL development
1. Update your graphics drivers.
2. Install the OpenCL drivers.
    *   For NVIDIA GPUs, these will be available as part of their CUDA Toolkit (available from [the NVIDIA CUDA ZONE](https://developer.nvidia.com/cuda-toolkit)). Make sure to indicate that you want to install the 'CUDA Toolkit' during the installation. After installing, the folder structure depicted below will probably be located at `C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v7.0\`.
    *   For AMD GPUs, these will be part of their AMD APP SDK (available from [the AMD website](http://developer.amd.com/tools-and-sdks/opencl-zone/amd-accelerated-parallel-processing-app-sdk/)). After installing, the folder structure depicted below will probably be located at `C:\Program Files (x86)\AMD APP SDK\2.9\opencl\`.
    *   When making use of integrated Intel GPUs, the OpenCL binaries will be available from [here](https://software.intel.com/en-us/articles/opencl-drivers). This has not been tested yet.

### Finding the locations of the needed files
The (incomplete) folder structure of your OpenCL files will probably look similar to this. It should pop up when you search for `OpenCL.lib` using the file explorer.
```
.
├── bin
├── docs
├── include
│   └── CL
│       └── cl.h
└── lib
    └── [platform]
        └── OpenCL.lib
```
You'll need to get the path to the folder containing `CL/cl.h` for the include directory setting and the path containing `OpenCL.lib` for the library directory setting. Furthermore, make sure that you pick the `OpenCL.lib` from the `lib/` directory and not the `OpenCL.dll` from the `bin/` directory. Even though both files are libraries, the `.dll` is used for dynamic linking, which is not what's used here (it's linked statically instead). The `[platform]` will most likely be `Win32`, but it might also be `x86`. If there is also a `x86_64` platform present, do not pick that one as this set-up is for 32-bit applications, which are not binary compatible with the 64-bit libraries.

### Configuring the project
1. Open the project solution file (`.sln`) in Visual Studio. You might be asked to upgrade the project to a newer version, this should be done.
2. Right click on the project 'EasyOpenCL' (in the project explorer) and go to 'Properties'
3. Make sure the 'Configuration:' is set to 'All Configurations'
4. Under 'Configuration Properties' -> 'VC++ Directories'
5. Edit the 'Libary Directories' to include the path to `OpenCL.lib` (click 'New Line' and enter the path)
6. Edit the 'Include Directors' to include the path to `CL/cl.h`.
7. In this example project, it has already been specified that `OpenCL.lib` should actually be linked to by the linker. However, if you set up your own project, you will also need to specify that. In order to do this, edit the 'Project Properties'. Under 'Configuration Properties' -> 'Linker' -> 'Input', add `OpenCL.lib` to the 'Additional Dependencies'.

### Building and running
Build and run the project (without debugger to prevent the console from closing immediately at the end) using `Ctrl-F5`. The program should compile, link and execute and display some list of numbers. If you want to debug your project, set some breakpoints and use `F5` instead of `Ctrl-F5`.

### Troubleshooting
######  Unable to find the path to `OpenCL.lib` or `CL/cl.h`
Make sure that you have installed an OpenCL-capable version of you graphics card driver. For AMD you'll need the AMD APP SDK (http://developer.amd.com/tools-and-sdks/opencl-zone/amd-accelerated-parallel-processing-app-sdk/). For NVIDIA, installing the CUDA-enabled drivers usually also installs OpenCL support.

######  Visual Studio error C1083: 'Cannot open include file `CL/cl.h`'
Make sure that the Include Directory has been specified correctly (step 5).

######  Visual Studio error LNK1104: 'Cannot open file `OpenCL.lib`'
Make sure that the Library Directory has been specified correctly (step 4).

######  Visual Studio error LNK2015: Undefined functions
You did not specify that the linker should link against `OpenCL.lib` or you specified the wrong folder (ie. compiling a 32-bit program whilst linking against the 64-bit library). 

###### `clCreateCommandQueueWithProperties` and `clCreateCommandQueue` 
`clCreateCommandQueue` has been deprecated in favour of `clCreateCommandQueueWithProperties` in the OpenCL 2.0 specification. Some devices do not support OpenCL 2.0 yet and they still follow to the older (OpenCL 1.1 or OpenCL 1.2) specification which does not include the depreciation of `clCreateCommandQueue`. If you experience errors about these functions: missing declarations or being deprecated, try the other function. The arguments are the same.

