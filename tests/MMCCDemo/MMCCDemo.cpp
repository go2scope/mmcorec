// MMCCDemo.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "MMCoreC.h"

using namespace std;

// throw exception on non-OK error code and retrieve error text
void processReturnCode(int returnCode) {
   const int MAX_MSG_LENGTH(1024);
   char errMsg[MAX_MSG_LENGTH];

   if (returnCode != g2s_OK) {
      g2s_getLastErrorText(errMsg, MAX_MSG_LENGTH);
      throw exception(errMsg);
   }
}


int main()
{

   try {
      cout << "Running MMCoreC Demo..." << endl;
      int ret = g2s_create_mmcc();
      processReturnCode(ret);

      const int MAX_MSG_LENGTH(1024);
      char msgBuffer[MAX_MSG_LENGTH];

      g2s_getVersionInfo(msgBuffer, MAX_MSG_LENGTH);
      cout << msgBuffer << endl;

      g2s_getAPIVersionInfo(msgBuffer, MAX_MSG_LENGTH);
      cout << msgBuffer << endl;


      cout << "Success!" << endl;
   }
   catch (exception e) {
      cout << e.what() << endl;
      cout << "Failed!" << endl;
   }

   g2s_delete_mmcc();
}
