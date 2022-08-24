// MMCCDemoDyn.cpp : This file contains the 'main' function. Program execution begins and ends there.
// Dynamically loads the MMCoreC library

#include <iostream>
#include "MMCoreC.h"

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h> // needed for dynamic loading of DLLs on Windows
#pragma warning(disable : 4996) // disable warning for deprecated CRT functions on Windows 
#endif

typedef int (*fn_create_mmcc)();
typedef int (*fn_delete_mmcc)();
typedef int (*fn_initialize)();

typedef int (*fn_load_device)(const char*, const char*, const char*);
typedef int (*fn_get_version_info)(char*, int);
typedef int (*fn_get_api_version_info)(char*, int);
typedef int (*fn_get_last_error_text)(char*, int);

typedef int (*fn_set_position)(const char*, double);
typedef int (*fn_get_position)(const char*, double*);

typedef int (*fn_device_busy)(const char*, int*);

using namespace std;

template<class T>
T getFunction(HMODULE handle, const char* functionName) {
   void* proc = GetProcAddress(handle, functionName);
   if (!proc)
      throw exception(functionName);

   return reinterpret_cast<T>(proc);
}

// throw exception on non-OK error code and retrieve error text
void processReturnCode(HMODULE mmccModule, int returnCode) {
   const int MAX_MSG_LENGTH(1024);
   char errMsg[MAX_MSG_LENGTH];
   fn_get_last_error_text get_last_error_text = getFunction<fn_get_last_error_text>(mmccModule, "g2s_getLastErrorText");

   if (returnCode != g2s_OK) {
      get_last_error_text(errMsg, MAX_MSG_LENGTH);
      throw exception(errMsg);
   }
}


int main()
{
   // load the module
   HMODULE mmcoreHandle = ::LoadLibrary(L"MMCoreC.dll");
   if (!mmcoreHandle) {
      DWORD errCode = GetLastError();
      cout << "Failed loading MMCoreC dynamic library, error code: " << errCode << endl;
      return 1;
   }

   fn_delete_mmcc delete_mmcc(nullptr);
   try {
      delete_mmcc = getFunction<fn_delete_mmcc>(mmcoreHandle, "g2s_delete_mmcc");
   }
   catch (exception e) {
      cout << e.what() << endl;
      return 1;
   }

   try {
      cout << "Running MMCoreC Demo..." << endl;

      // load functions from the mmcc module
      fn_create_mmcc create_mmcc = getFunction<fn_create_mmcc>(mmcoreHandle, "g2s_create_mmcc");
      fn_get_version_info get_version_info = getFunction<fn_get_version_info>(mmcoreHandle, "g2s_getVersionInfo");
      fn_get_api_version_info get_api_version_info = getFunction<fn_get_api_version_info>(mmcoreHandle, "g2s_getAPIVersionInfo");
      fn_load_device load_device = getFunction<fn_load_device>(mmcoreHandle, "g2s_load_device");
      fn_initialize initialize = getFunction<fn_initialize>(mmcoreHandle, "g2s_initialize");
      fn_set_position set_position = getFunction<fn_set_position>(mmcoreHandle, "g2s_setPosition");
      fn_get_position get_position = getFunction<fn_get_position>(mmcoreHandle, "g2s_getPosition");
      fn_device_busy device_busy = getFunction<fn_device_busy>(mmcoreHandle, "g2s_deviceBusy");

      // create mmcc instance
      create_mmcc();

      const int MAX_MSG_LENGTH(1024);
      char msgBuffer[MAX_MSG_LENGTH];

      get_version_info(msgBuffer, MAX_MSG_LENGTH);
      cout << msgBuffer << endl;

      get_api_version_info(msgBuffer, MAX_MSG_LENGTH);
      cout << msgBuffer << endl;

      // load stage device
      string devName("SingleAxisStage");
      int ret = load_device(devName.c_str(), "DemoCamera", "DStage");
      if (ret != g2s_OK)
         processReturnCode(mmcoreHandle, ret);

      ret = initialize();
      if (ret != g2s_OK)
         processReturnCode(mmcoreHandle, ret);

      // move stage
      ret = set_position(devName.c_str(), 10.35);
      if (ret != g2s_OK)
         processReturnCode(mmcoreHandle, ret);

      // wait until the stage stops
      int busy(0);
      device_busy(devName.c_str(), &busy);
      while (busy != 0) {
         cout << "Waiting for stage..." << endl;
         device_busy(devName.c_str(), &busy);
         Sleep(50); // ms
      }

      // get position
      double posUm(0.0);
      ret = get_position(devName.c_str(), &posUm);
      if (ret != g2s_OK)
         processReturnCode(mmcoreHandle, ret);

      cout << "Position of " << devName << " is " << posUm << endl;

      cout << "Success!" << endl;

   }
   catch (exception e) {
      cout << e.what() << endl;
      cout << "Failed!" << endl;
   }

   // delete mmcc instance
   delete_mmcc();

   return 0;
}
