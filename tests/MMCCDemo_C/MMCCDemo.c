#undef __cplusplus
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <process.h>

#include "MMCoreC.h"

// report errors on stdout
void reportLastError(int code)
{
   char errMsg[g2s_MAX_MESSAGE_LENGTH];
   g2s_getLastErrorText(errMsg, g2s_MAX_MESSAGE_LENGTH);
   printf("Error code %d, message: %s\n", code, errMsg);
}

int main(int argc, char* argv[])
{
   // initialize
   printf("Initializing MMCore...\n");
   int ret = g2s_OK;
   ret = g2s_create_mmcc();
   if (ret != g2s_OK)
   {
      reportLastError(ret);
      g2s_delete_mmcc();
      return ret;
   }

   ret = g2s_load_device("Camera", "DemoCamera", "DCam");
   if (ret != g2s_OK)
   {
      reportLastError(ret);
      g2s_delete_mmcc();
      return ret;
   }

   ret = g2s_initialize_all_devices();
   if (ret != g2s_OK)
   {
      reportLastError(ret);
      g2s_delete_mmcc();
      return ret;
   }

   printf("Successfully initialized.\n");

   unsigned circularBufferCapacity = 0;
   g2s_getBufferTotalCapacity(&circularBufferCapacity);
   // NOTE: capacity is returned in the number of images of current size
   if (circularBufferCapacity < 50)
   {
      // if the buffer is too shallow make it deeper, just in case
      // but we can only specify the total memory size, not the number of images
      g2s_setCircularBufferMemoryFootprint(500); // 500MB buffer requested
   }

   // get basic camera info
   unsigned int width, height, bytesPerPixel, bitsPerPixel;
   size_t currentSize = 0;
   g2s_getImageWidth(&width);
   g2s_getImageHeight(&height);
   g2s_getBytesPerPixel(&bytesPerPixel);
   g2s_getImageBitDepth(&bitsPerPixel);
   g2s_getImageBufferSize(&currentSize);

   // camera name, used later as a handle in some functions
   char camera[g2s_MAX_MESSAGE_LENGTH];
   ret = g2s_getCameraDevice(camera, g2s_MAX_MESSAGE_LENGTH);
   if (ret != g2s_OK)
   {
      reportLastError(ret);
      goto cleanup;
   }

   printf("\nCamera device: %s\n", camera);
   printf("Image dimensions: %d X %d X %d, dynamic range: %d bpp\n", width, height, bytesPerPixel, bitsPerPixel);

   // discover all available device properties (camera settings)
   printf("\nCamera properties: \n");
   size_t numProps = 0;
   ret = g2s_getNumberOfDeviceProperties(camera, &numProps);
   if (ret != g2s_OK)
   {
      reportLastError(ret);
      goto cleanup;
   }

   // allocate string array to hold properties
   char** propNames;
   propNames = malloc(numProps * sizeof(char*));
   for (size_t i=0; i<numProps; i++)
   {
      propNames[i] = malloc(g2s_MAX_MESSAGE_LENGTH);
      propNames[i][0] = 0;
   }

   // retrieve properties and print them
   ret = g2s_getDevicePropertyNames(camera, propNames, numProps, g2s_MAX_MESSAGE_LENGTH);
   if (ret != g2s_OK)
   {
      reportLastError(ret);
      goto cleanup;
   }

   for (size_t i=0; i<numProps; i++)
   {
      char value[g2s_MAX_MESSAGE_LENGTH];
      g2s_getProperty(camera, propNames[i], value, g2s_MAX_MESSAGE_LENGTH);
      printf("%s = %s\n", propNames[i], value);
      free(propNames[i]);
   }
   free(propNames);

   // acquire single image
   // --------------------
   printf("\nSingle image snap...\n");
   ret = g2s_snapImage();
   if (ret != g2s_OK)
   {
      reportLastError(ret);
      goto cleanup;
   }

   g2s_getImageBufferSize(&currentSize);
   unsigned char* imgBuf = 0;
   imgBuf = (unsigned char*) malloc(currentSize);
   ret = g2s_getImage(imgBuf, currentSize);
   // do something with the image buffer ...
   free(imgBuf); // free it after we are done
   imgBuf = 0;

   if (ret != g2s_OK)
   {
      reportLastError(ret);
      goto cleanup;
   }
   printf("We acquired a single image. Transferred %zd image bytes\n", currentSize);
   
   g2s_sleep(1000);

   // acquire live image stream
   // -------------------------

   ret = g2s_startContinuousSequenceAcquisition(0.0);
   if (ret != g2s_OK)
   {
      reportLastError(ret);
      goto cleanup;
   }

   printf("Acquiring continuous sequence...\n");
   g2s_bool acqRunning = g2s_false;
   g2s_isSequenceRunning(&acqRunning);

   // acquire images from the circular buffer
   int numImages = 10;
   int imageCount = 0;

   while (imageCount < numImages && acqRunning)
   {
      g2s_getImageBufferSize(&currentSize);
      int imagesWaiting = 0;
      g2s_getRemainingImageCount(&imagesWaiting);
      if (imagesWaiting > 0) {
         imgBuf = (unsigned char*)malloc(currentSize);
         ret = g2s_popNextImage(imgBuf, currentSize);
         imageCount++;
         printf("Image %d retrieved, waiting in buffer %d\n", imageCount, imagesWaiting);
         free(imgBuf);

         if (ret != g2s_OK)
         {
            reportLastError(ret);

            // stop and clean up after fatal error
            g2s_stopSequenceAcquisition();
            goto cleanup;
         }
      }

      g2s_sleep(50); // wait a bit before trying again
      g2s_isSequenceRunning(&acqRunning);
   }

   if (imageCount < numImages) {
      printf("\nSequence failed, %d images acquired out of %d", imageCount, numImages);
   }

   printf("\nFinished.");

   // disconnect and clean up
   // -----------------------
   cleanup:

   // release dll instance  
   g2s_delete_mmcc();
	return ret;
}

