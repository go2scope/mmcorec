# mmcorec
*MMCoreC* is an experimental C lanugage interface to micro-manager MMCore. Wraps MMCore C++ static library into a C interface DLL. C header eliminates all 3-rd party dependencies and provides better compatibility with other languages.

Currently provides only a small subset of the MMCore API. Intended for the proof of concept only.

Use Visual Studio 2019 to build the project.

To test with C++ use *MMCCDemo* project. A trivial example of how to create a project that uses MMCoreC with C++.

To test with C use *MMCCDemo_C* project. This example is more involved than the one above as it exercises the camera API. If you just build and run, there will be errors, as the the project does not contain any device adapters. To run without errors this project requires that mmgr_dal_DemoCamera.dll is present in the working directory. This file shold be manually copied from the original micro-manager distribution or from the mmcoreAndDevices sub-module, after the build has finished.


