## Visual Studio example project
You will need so set up the project before you can build and run this example. The confinguration still needs the include location for the OpenCL headers and the corresponding implementation library. These should be installed by your GPU driver.

### Finding the locations of the needed files
The (incomplete) folder structure of your OpenCL files will probably look similar to this. It should pop up when you search for `OpenCL.lib` using the file explorer.
```
.
├── bin
│   └── x86
│       └── OpenCL.dll
├── docs
├── include
│   └── CL
│       └── cl.h
└── lib
    ├── x86
    │   └── OpenCL.lib
    └── x86_64
        └── OpenCL.lib
```
You'll need to get the path to the folder containing `CL/cl.h` for the include directory setting and the path containing `OpenCL.lib` (in the x86 folder for 32-bit programs, pick the one in the x64_64 folder only for 64-bit programs) for the library directory setting. This example uses static linking (at compile-time), therefore you will use the library in the `lib/` directory. If you wanted to link dynamically (at load-time or run-time), you would use `OpenCL.dll` from the `bin/` directory, but that is not supported yet by this setup.

### Configuring the project
1. Right click on the project 'EasyOpenCL' (in the project explorer) and go to 'Properties'
2. Make sure the 'Configuration:' is set to 'All Configurations'
3. Under 'Configuration Properties' -> 'VC++ Directories'
4. Edit the 'Libary Directories' to include the path to `OpenCL.lib` (click 'New Line' and enter the path)
5. Edit the 'Include Directors' to include the path to `CL/cl.h`.
6. In this example project, it has already been specified that `OpenCL.lib` should actually be linked to by the linker. However, if you set up your own project, you will also need to specify that. In order to do this, edit the 'Project Properties'. Under 'Configuration Properties' -> 'Linker' -> 'Input', add `OpenCL.lib` to the 'Additional Dependencies'.

### Building and running
Build and run the project (without debugger to prevent the console from closing immediately at the end) using `Ctrl-F5`. If you want to debug your project, set some breakpoints and use `F5`.

### Troubleshooting
######  Unable to find the path to `OpenCL.lib` or `CL/cl.h`
Make sure that you have installed an OpenCL-capable version of you graphics card driver. For AMD you'll need the AMD APP SDK (http://developer.amd.com/tools-and-sdks/opencl-zone/amd-accelerated-parallel-processing-app-sdk/). For NVIDIA, installing the CUDA-enabled drivers usually also installs OpenCL support.

######  Visual Studio error C1083: 'Cannot open include file `CL/cl.h`'
Make sure that the Include Directory has been specified correctly (step 5).

######  Visual Studio error LNK1104: 'Cannot open file `OpenCL.lib`'
Make sure that the Library Directory has been specified correctly (step 4).

######  Visual Studio error LNK2015: Undefined functions
You did not specify that the linker should link against `OpenCL.lib` or you specified the wrong folder (ie. compiling a 32-bit program whilst linking against the 64-bit library). 



