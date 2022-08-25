// MMCCDemoDyn.cpp :
// Dynamically loads the MMCoreC library and demonstrates how to use the API

#include <iostream>
#include "MMCoreC.h"

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h> // needed for dynamic loading of DLLs on Windows
#pragma warning(disable : 4996) // disable warning for deprecated CRT functions on Windows 
#endif

using namespace std;

// obtain typed function pointer from the dynamically loaded module
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
   auto get_last_error_text = getFunction<fn_get_last_error_text>(mmccModule, "g2s_getLastErrorText");

   if (returnCode != g2s_OK) {
      get_last_error_text(errMsg, MAX_MSG_LENGTH);
      throw exception(errMsg);
   }
}


// ----
// MAIN
// ----
int main()
{
   // load the MMCoreC module
   HMODULE mmcoreHandle = ::LoadLibrary(L"MMCoreC.dll");
   if (!mmcoreHandle) {
      DWORD errCode = GetLastError();
      cout << "Failed loading MMCoreC dynamic library, error code: " << errCode << endl;
      return 1;
   }

   // first get the delete method to facilitate error exit handling
   fn_delete_mmcc delete_mmcc(nullptr);
   try {
      delete_mmcc = getFunction<fn_delete_mmcc>(mmcoreHandle, "g2s_delete_mmcc");
   }
   catch (exception e) {
      cout << e.what() << endl;
      return 1;
   }

   // Perform the demo task
   try {
      cout << "Running MMCoreC Demo..." << endl;

      // load functions from the mmcc module
      auto create_mmcc = getFunction<fn_create_mmcc>(mmcoreHandle, "g2s_create_mmcc");
      auto get_version_info = getFunction<fn_get_version_info>(mmcoreHandle, "g2s_getVersionInfo");
      auto get_api_version_info = getFunction<fn_get_api_version_info>(mmcoreHandle, "g2s_getAPIVersionInfo");
      auto load_device = getFunction<fn_load_device>(mmcoreHandle, "g2s_load_device");
      auto initialize = getFunction<fn_initialize>(mmcoreHandle, "g2s_initialize");
      auto set_position = getFunction<fn_set_position>(mmcoreHandle, "g2s_setPosition");
      auto get_position = getFunction<fn_get_position>(mmcoreHandle, "g2s_getPosition");
      auto device_busy = getFunction<fn_device_busy>(mmcoreHandle, "g2s_deviceBusy");

      // create mmcc instance
      create_mmcc();

      // get version info to verify the basic function
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

      // initialize loaded devices
      ret = initialize();
      if (ret != g2s_OK)
         processReturnCode(mmcoreHandle, ret);

      // move stage
      // this is typically a non-blocking call (but depends on the driver impl.)
      ret = set_position(devName.c_str(), 10.35);
      if (ret != g2s_OK)
         processReturnCode(mmcoreHandle, ret);

      // wait until the stage stops
      int busy(0);
      device_busy(devName.c_str(), &busy);
      while (busy) {
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
