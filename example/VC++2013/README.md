/*
    // WINDOWS (VISUAL STUDIO)
    The include file directory is already set properly, but you need to set up your compiler to be able to find OpenCL.lib
    This files differs for every GPU and also requires other libraries, therefore I can't just include it on Blackboard.

    To do this:
    1.Using your file explorer, find the 32-bit OpenCL.lib on your system
        If you can't find it, make sure you install an OpenCL-enabled version of your video card driver.
        For AMD, install the AMD APP SDK (http://developer.amd.com/tools-and-sdks/opencl-zone/amd-accelerated-parallel-processing-app-sdk/)
    2. Right click on the project 'OpenCL-Framework' (in the project explorer) and go to 'Properties'
    3. Make sure the 'Configuration:' is set to 'All Configurations'
    4. Under 'Configuration Properties' -> 'VC++ Directories'
    5. Edit the 'Libary Directories' to include the path to 'OpenCL.lib' (click 'New Line' and enter the path)

    6. Build and run the program (without the debugger to prevent the console from closing immediately at the end) using Ctrl-F5
*/