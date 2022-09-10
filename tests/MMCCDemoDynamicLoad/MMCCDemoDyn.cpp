// MMCCDemoDyn.cpp :
// Dynamically loads the MMCoreC library and demonstrates how to use the API

#include <iostream>
#include <vector>
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
   HMODULE mmcoreHandle = ::LoadLibraryW(L"MMCoreC.dll");
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

      // load some functions from the mmcc module
      auto create_mmcc = getFunction<fn_create_mmcc>(mmcoreHandle, "g2s_create_mmcc");
      auto get_version_info = getFunction<fn_get_version_info>(mmcoreHandle, "g2s_getVersionInfo");
      auto get_api_version_info = getFunction<fn_get_api_version_info>(mmcoreHandle, "g2s_getAPIVersionInfo");
      auto load_device = getFunction<fn_load_device>(mmcoreHandle, "g2s_load_device");
      auto initialize_all_devices = getFunction<fn_initialize_all_devices>(mmcoreHandle, "g2s_initialize_all_devices");
      auto set_position = getFunction<fn_set_position>(mmcoreHandle, "g2s_setPosition");
      auto get_position = getFunction<fn_get_position>(mmcoreHandle, "g2s_getPosition");
      auto device_busy = getFunction<fn_device_busy>(mmcoreHandle, "g2s_deviceBusy");
      auto get_loaded_devices_of_type = getFunction<fn_get_loaded_devices_of_type>(mmcoreHandle, "g2s_getLoadedDevicesOfType");
      auto get_loaded_devices = getFunction<fn_get_loaded_devices>(mmcoreHandle, "g2s_getLoadedDevices");
      auto load_system_configuration = getFunction<fn_load_system_configuration>(mmcoreHandle, "g2s_loadSystemConfiguration");

      // create mmcc instance (singleton)
      create_mmcc();

      // get version info to verify the basic function
      const int MAX_MSG_LENGTH(1024);
      char msgBuffer[MAX_MSG_LENGTH];

      get_version_info(msgBuffer, MAX_MSG_LENGTH);
      cout << msgBuffer << endl;

      get_api_version_info(msgBuffer, MAX_MSG_LENGTH);
      cout << msgBuffer << endl;

      // load configuration
      int ret = load_system_configuration("cpx_demo.cfg");
      if (ret != g2s_OK)
         processReturnCode(mmcoreHandle, ret);

      vector<char*> devLabels(100);
      for (size_t i = 0; i < devLabels.size(); i++)
      {
         devLabels[i] = new char[g2s_MAX_MESSAGE_LENGTH];
         devLabels[i][0] = '\0';
      }

      ret = get_loaded_devices(&devLabels[0], devLabels.size(), g2s_MAX_MESSAGE_LENGTH);

      int index(0);
      cout << "Available devices: " << endl;
      while (devLabels[index][0] != '\0' && index < devLabels.size()) {
         cout << "\t" << devLabels[index++] << endl;
      }

      // free string buffer
      for (size_t i = 0; i < devLabels.size(); i++)
      {
         delete devLabels[i];
      }

      if (ret != g2s_OK)
         processReturnCode(mmcoreHandle, ret);

      string devName("Z");

      // move stage
      // this is typically a non-blocking call (but depends on the driver impl.)
      ret = set_position(devName.c_str(), 10.35); // um
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
