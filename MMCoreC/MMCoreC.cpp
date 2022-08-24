//MMCoreC.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "MMCoreC.h"
#include "MMCore\MMCore.h"
#include "MMCore\Error.h"
#include <string>

#ifdef WIN32
#define STRCPYFUNC(dest,size,src) strcpy_s(dest,size,src)
#else
#define STRCPYFUNC(dest,size,src) strlcpy(dest,src,size)
#endif

using namespace std;
CMMCore* core = 0;

string lastErrorText;
int lastErrorCode(0);
   
int G2S_OK()
{
   return g2s_resetError();
}

int G2S_ERROR(CMMError& e)
{
   lastErrorText = e.what();
   lastErrorCode = e.getCode();
   return lastErrorCode;
}

int G2S_ERROR(int code, const char* text)
{
   lastErrorCode = code;
   lastErrorText = text;
   return code;
}

#define CHECK_INSTANCE if (core == 0) return G2S_ERROR(g2s_INVALID_INSTANCE, "MMCoreC instance not initialized.")


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/**
 * Creates an instance of the G2SClient.
 * The instance is a singleton.
 */
G2SCLIENTC_API g2s_create_mmcc()
{
   if (core == 0)
   {
      core = new CMMCore();
      return G2S_OK();
   }
   
   return G2S_ERROR(g2s_INSTANCE_EXISTS, "Instance already exists");
}

/**
 * Deletes the go2scope instance.
 * If instance does not exist, no action will be taken.
 */
G2SCLIENTC_API g2s_delete_mmcc()
{
   delete core;
   core = 0;

   return G2S_OK();
}

G2SCLIENTC_API g2s_initialize()
{
   CHECK_INSTANCE;
   try
   {
      core->initializeAllDevices();
   }
   catch (CMMError& e)
   {
      return G2S_ERROR(e);
   }
   return G2S_OK();
}

G2SCLIENTC_API g2s_load_device(const char* label, const char* moduleName, const char* deviceName)
{
   CHECK_INSTANCE;
   try
   {
      core->loadDevice(label, moduleName, deviceName);
   }
   catch (CMMError& e)
   {
      return G2S_ERROR(e);
   }
   return G2S_OK();
}

G2SCLIENTC_API g2s_getVersionInfo(char* info, int maxLength)
{
   CHECK_INSTANCE;
   string verInfo = core->getVersionInfo();
   strcpy_s(info, maxLength, verInfo.c_str());
   return G2S_OK();
}

G2SCLIENTC_API g2s_getAPIVersionInfo(char* info, int maxLength)
{
   CHECK_INSTANCE;
   string verInfo = core->getAPIVersionInfo();
   strcpy_s(info, maxLength, verInfo.c_str());
   return G2S_OK();
}

/**
 * Returns the text message of the last error.
 * Empty string if no errors.
 */
G2SCLIENTC_API g2s_getLastErrorText(char* errMessage, int maxLength)
{
#ifdef WIN32
   strncpy_s(errMessage, maxLength, lastErrorText.c_str(), _TRUNCATE);
#else
	strncpy(errMessage, lastErrorText.c_str(), maxLength);
#endif
   return G2S_OK();
}

/**
 * Returns the last error code.
 */
G2SCLIENTC_API g2s_getLastErrorCode()
{
   return lastErrorCode;
}

/**
 * Resets error info to "no error" state.
 * Generally there is no need to call this function,
 * since all commands reset error code prior to execution.
 */
G2SCLIENTC_API g2s_resetError()
{
   lastErrorCode = g2s_OK;
   lastErrorText.clear();
   return lastErrorCode;
}

/**
 * Sets camera exposure in milliseconds.
 * @param expMs - exposure in milliseconds
 */
G2SCLIENTC_API g2s_setExposure(double expMs)
{
   CHECK_INSTANCE;
   try
   {
      core->setExposure(expMs);
   }
   catch(CMMError& e)
   {
      return G2S_ERROR(e);
   }
   return G2S_OK();
}

/**
 * Returns camera exposure in milliseconds 
 * @param expMs - current exposure in milliseconds
 */
G2SCLIENTC_API g2s_getExposure(double* expMs)
{
   CHECK_INSTANCE;
   try
   {
      *expMs = core->getExposure();
   }
   catch(CMMError& e)
   {
      return G2S_ERROR(e);
   }
   return G2S_OK();
}

/**
 * Retrieves the current image and copies into the imgBuf.
 * Caller is responsible for allocating the buffer of adequate size before calling this function.
 * The function does not automatically check whether bufSize corresponds to the actual image size.
 * If bufSize argument is larger than the actual image, the program will crash.
 *
 * Applies only to images obtained by "Snap" command, not to the live stream images.
 *
 * @param imgBuf - buffer to which the image will be copied to.
 * @param bufSize - the size of the destination buffer, it must be equal to the actual image size
 */
G2SCLIENTC_API g2s_getImage(unsigned char* imgBuf, size_t bufSize)
{
   CHECK_INSTANCE;
   try
   {
      memmove(imgBuf, core->getImage(), bufSize);
   }
   catch(CMMError& e)
   {
      return G2S_ERROR(e);
   }
   return G2S_OK();
}

/**
 * Snaps a single image.
 * Single image is acquired and stored in the internal buffer.
 * The image can be retreived with getImage() function.
 */
G2SCLIENTC_API g2s_snapImage()
{
   CHECK_INSTANCE;
   try
   {
      core->snapImage();
   }
   catch(CMMError& e)
   {
      return G2S_ERROR(e);
   }
   return G2S_OK();
}

/**
 * Returns the current image width in pixels.
 * @param w - image width in pixels
 */
G2SCLIENTC_API g2s_getImageWidth(unsigned* w)
{
   CHECK_INSTANCE;
   try
   {
      *w = core->getImageWidth();
   }
   catch(CMMError& e)
   {
      return G2S_ERROR(e);
   }
   return G2S_OK();
}

/**
 * Returns the current image height in pixels. 
 * @param h - image height in pixels
 */
G2SCLIENTC_API g2s_getImageHeight(unsigned* h)
{
   CHECK_INSTANCE;
   try
   {
      *h = core->getImageHeight();
   }
   catch(CMMError& e)
   {
      return G2S_ERROR(e);
   }
   return G2S_OK();
}

/**
 * Returns the current pixel depth in bytes.
 * @param bytesPP - number of bytes per pixel, usually 1 (8-bit grayscale), 2 (16-bit grayscale) or 4 (32-bit color RGBA)
 */
G2SCLIENTC_API g2s_getBytesPerPixel(unsigned* bytesPP)
{
   CHECK_INSTANCE;
   try
   {
      *bytesPP = core->getImageHeight();
   }
   catch(CMMError& e)
   {
      return G2S_ERROR(e);
   }
   return G2S_OK();
}

/**
 * Returns the current dynamic range as bits-per-pixel.
 * @param bitsPP - bits per pixel, defines the range of pixel values
 */
G2SCLIENTC_API g2s_getImageBitDepth(unsigned* bitsPP)
{
   CHECK_INSTANCE;
   try
   {
      *bitsPP = core->getImageBitDepth();
   }
   catch(CMMError& e)
   {
      return G2S_ERROR(e);
   }
   return G2S_OK();
}

/**
 * Returns the number of color components of each pixel.
 * This function is commonly used to determine whether we are dealing with
 * grayscale or color camera.
 *
 * 1 - means grayscale image
 * 3 - means RGB
 * 4 - means RGBA
 *
 * @param numComp - number of color components
 */
G2SCLIENTC_API g2s_getNumberOfComponents(unsigned* numComp)
{
   CHECK_INSTANCE;
   try
   {
      *numComp = core->getNumberOfComponents();
   }
   catch(CMMError& e)
   {
      return G2S_ERROR(e);
   }
   return G2S_OK();
}

/**
 * Returns the number of channels in the camera
 * Ordinary cameras have only one channel. Special cameras may have more.
 * @param numChan - number of color components
 */
G2SCLIENTC_API g2s_getNumberOfCameraChannels(unsigned* numChan)
{
   CHECK_INSTANCE;
   try
   {
      *numChan = core->getNumberOfCameraChannels();
   }
   catch(CMMError& e)
   {
      return G2S_ERROR(e);
   }
   return G2S_OK();
}

/**
 * Retrieves the channel name.
 * @param chan - channel index, zero based
 * @param name - channel name
 * @param maxLength - maximum length of the name string
 */
G2SCLIENTC_API g2s_getCameraChannelName(unsigned chan, char* name, size_t maxLength)
{
   CHECK_INSTANCE;
   try
   {
      string chName = core->getCameraChannelName(chan);
      STRCPYFUNC(name, maxLength, chName.c_str());
   }
   catch(CMMError& e)
   {
      return G2S_ERROR(e);
   }
   return G2S_OK();

}

/**
 * Returns the current image size, in bytes.
 * This number is always equal to imageWidth * imageHeight * bytesPerPixel
 * @param bufSize - image size in bytes
 */
G2SCLIENTC_API g2s_getImageBufferSize(size_t* bufSize)
{
   CHECK_INSTANCE;
   try
   {
      *bufSize = (size_t)core->getImageBufferSize();
   }
   catch(CMMError& e)
   {
      return G2S_ERROR(e);
   }
   return G2S_OK();
}

/**
 * Returns the number of device properties.
 *
 * @param label - device name (label)
 * @param num - number of properties
 */
G2SCLIENTC_API g2s_getNumberOfDeviceProperties(const char* label, size_t* num)
{
   CHECK_INSTANCE;
   try
   {
      vector<string> devNames = core->getDevicePropertyNames(label);
      *num = devNames.size();
   }
   catch(CMMError& e)
   {
      return G2S_ERROR(e);
   }
   return G2S_OK();
}

/**
 * Populates an array of strings passed as an input parameter, with property names.
 * The caller must allocate the array of strings buffer (char** names) before calling
 * this function.
 *
 * @param label - device name (label)
 * @param names - array of strings, allocated and provided by the caller
 * @param numNames - the size of the array
 * @param maxLength - maximum length of each string
 */
G2SCLIENTC_API g2s_getDevicePropertyNames(const char* label, char** names, size_t numNames, size_t maxLength )
{
   CHECK_INSTANCE;
   try
   {
      vector<string> devNames = core->getDevicePropertyNames(label);
      for (int i=0; i<min(numNames, devNames.size()); i++)
      {
         STRCPYFUNC((names)[i], min(devNames[i].size() + 1, maxLength), devNames[i].c_str());
      }
   }
   catch(CMMError& e)
   {
      return G2S_ERROR(e);
   }
   return G2S_OK();
}

/**
 * Returns the value of the specified property for a given device.
 *
 * @param label - device name (label)
 * @param propName - property name
 * @param value - property value
 * @param maxLength - maximum length of the value string
 */
G2SCLIENTC_API g2s_getProperty(const char* label, const char* propName, char* value, int maxLength)
{
   CHECK_INSTANCE;
   try
   {
      STRCPYFUNC(value, maxLength, core->getProperty(label, propName).c_str());
   }
   catch(CMMError& e)
   {
      return G2S_ERROR(e);
   }
   return G2S_OK();
}

/**
 * Returns the value of the specified property for a given device, cached by the server.
 * This is much faster than getProperty() because it does not actually query the device,
 * but not always accurate.
 *
 * @param label - device name (label)
 * @param propName - property name
 * @param value - property value
 * @param maxLength - maximum length of the value string
 */
G2SCLIENTC_API g2s_getPropertyFromCache(const char* label, const char* propName, char* value, int maxLength)
{
   CHECK_INSTANCE;
   try
   {
      STRCPYFUNC(value, maxLength, core->getPropertyFromCache(label, propName).c_str());
   }
   catch(CMMError& e)
   {
      return G2S_ERROR(e);
   }
   return G2S_OK();
}

/**
 * Sets the value for the specified property.
 * @param label - device name (label)
 * @param propName - property name
 * @param value - property value
 * @param maxLength - maximum length of the value string
 */
G2SCLIENTC_API g2s_setProperty(const char* label, const char* propName, const char* value)
{
   CHECK_INSTANCE;
   try
   {
      core->setProperty(label, propName, value);
   }
   catch(CMMError& e)
   {
      return G2S_ERROR(e);
   }
   return G2S_OK();
}

/**
 * Tells us whether the property with specified name is available.
 * @param hasProperty - true if property exists
 */
G2SCLIENTC_API g2s_hasProperty(const char* label, const char* propName, g2s_bool* hasProperty)
{
   CHECK_INSTANCE;
   try
   {
      *hasProperty = core->hasProperty(label, propName) ? g2s_true : g2s_false;
   }
   catch(CMMError& e)
   {
      return G2S_ERROR(e);
   }
   return G2S_OK();
}

/**
 * Returns the number of allowed property values.
 * If it returns 0 values, it means that the property has no restriction in the number
 * of different values it accepts.
 * @param label - device name (label)
 * @param propName - property name
 * @param num - number of values (0 if unlimited)
 */
G2SCLIENTC_API g2s_getNumberOfAllowedPropertyValues(const char* label, const char* propName, size_t* num)
{
   CHECK_INSTANCE;
   try
   {
      vector<string> propValues = core->getAllowedPropertyValues(label, propName);
      *num = propValues.size();
   }
   catch(CMMError& e)
   {
      return G2S_ERROR(e);
   }
   return G2S_OK();
}

/**
 * Returns a set of allowed property values. This is useful for populating drop-downs in the GUI.
 * The number of allowed values can be obtained with g2s_getNumberOfAllowedPropertyValues().
 * The caller must allocate the "char** values" array of strings before calling this function.
 *
 * @param label - device name (label)
 * @param propName - property name
 * @param values - array of strings representing allowed values
 * @param numValues - size of the array of strings
 * @param maxLength - maximum length of each string
 */
G2SCLIENTC_API g2s_getAllowedPropertyValues(const char* label, const char* propName, char** values, size_t numValues, int maxLength)
{
   CHECK_INSTANCE;
   try
   {
      vector<string> propValues = core->getAllowedPropertyValues(label, propName);
      for (int i=0; i<min(numValues, propValues.size()); i++)
      {
			STRCPYFUNC(values[i], (size_t)min((int)propValues[i].size() + 1, maxLength), propValues[i].c_str());
      }
   }
   catch(CMMError& e)
   {
      return G2S_ERROR(e);
   }
   return G2S_OK();
}

/**
 * Tells us whether the property is read only.
 * @param label - device name (label)
 * @param propName - property name
 * @param readonly - true if property is read-only
 */
G2SCLIENTC_API g2s_isPropertyReadOnly(const char* label, const char* propName, g2s_bool* readonly)
{
   CHECK_INSTANCE;
   try
   {
      *readonly = core->isPropertyReadOnly(label, propName) ? g2s_true : g2s_false;
   }
   catch(CMMError& e)
   {
      return G2S_ERROR(e);
   }
   return G2S_OK();
}

/**
 * Tells us whether the property has restricted range of values (limits).
 * @param label - device name (label)
 * @param propName - property name
 * @param hasLimits - true if property has limited range
 */
G2SCLIENTC_API g2s_hasPropertyLimits(const char* label, const char* propName, g2s_bool* hasLimits)
{
   CHECK_INSTANCE;
   try
   {
      *hasLimits = core->hasPropertyLimits(label, propName) ? g2s_true : g2s_false;
   }
   catch(CMMError& e)
   {
      return G2S_ERROR(e);
   }
   return G2S_OK();
}

/**
 * Returns the lower limit for property values. Makes sense only for numeric properties
 * and if g2s_hasPropertyLimits() returns true.
 * @param label - device name (label)
 * @param propName - property name
 * @param limit - lower limit value
 */
G2SCLIENTC_API g2s_getPropertyLowerLimit(const char* label, const char* propName, double* limit)
{
   CHECK_INSTANCE;
   try
   {
      *limit = core->getPropertyLowerLimit(label, propName);
   }
   catch(CMMError& e)
   {
      return G2S_ERROR(e);
   }
   return G2S_OK();
}

/**
 * Returns the upper limit for property values. Makes sense only for numeric properties
 * and if g2s_hasPropertyLimits() returns true.
 * @param label - device name (label)
 * @param propName - property name
 * @param limit - upper limit value
 */
G2SCLIENTC_API g2s_getPropertyUpperLimit(const char* label, const char* propName, double* limit)
{
   CHECK_INSTANCE;
   try
   {
      *limit = core->getPropertyUpperLimit(label, propName);
   }
   catch(CMMError& e)
   {
      return G2S_ERROR(e);
   }
   return G2S_OK();
}

/**
 * Returns the property type: floating point, integer or string.
 * @param label - device name (label)
 * @param propName - property name
 * @param type - property type
 */
G2SCLIENTC_API g2s_getPropertyType(const char* label, const char* propName, enum g2s_PropertyType* type)
{
   CHECK_INSTANCE;
   // TODO
   /*
   try
   {
      g2s::client::G2SPROPERTYTYPE mmType = core->propertyGetType(string(label), string(propName));
      switch (mmType)
      {
      case g2s::client::G2SPROPERTYTYPE::UNDEFINED:
         *type = Undef;
         break;
      case g2s::client::G2SPROPERTYTYPE::DECIMAL:
         *type = Float;
         break;
      case g2s::client::G2SPROPERTYTYPE::STRING:
         *type = String;
         break;
      case g2s::client::G2SPROPERTYTYPE::INTEGER:
         *type = Integer;
         break;
      default:
         throw CMMError("Unknown property type");
      }
   }
   catch(CMMError& e)
   {
      return G2S_ERROR(e);
   }
   */
   return G2S_OK();
}

/**
 * Tells us whether device is busy.
 * @param deviceName - device name (label)
 * @param busy - true if busy
 */
G2SCLIENTC_API g2s_deviceBusy(const char* deviceName, g2s_bool* busy)
{
   CHECK_INSTANCE;
   try
   {
      *busy = core->deviceBusy(deviceName) ? g2s_true : g2s_false;
   }
   catch(CMMError& e)
   {
      return G2S_ERROR(e);
   }
   return G2S_OK();
}

G2SCLIENTC_API g2s_getFocusDevice(char* buffer, size_t maxLength)
{
   CHECK_INSTANCE;
   try
   {
      string focusDev = core->getFocusDevice();
      if (focusDev.size() >= maxLength)
         throw CMMError("Name too long to fit in the buffer");
      STRCPYFUNC(buffer, maxLength, focusDev.c_str());
   }
   catch (CMMError& e)
   {
      return G2S_ERROR(e);
   }
   return G2S_OK();
}

G2SCLIENTC_API g2s_setFocusDevice(const char* buffer)
{
   CHECK_INSTANCE;
   try
   {
      core->setFocusDevice(buffer);
   }
   catch (CMMError& e)
   {
      return G2S_ERROR(e);
   }
   return G2S_OK();
}

G2SCLIENTC_API g2s_setPosition(const char* stageLabel, double position)
{
   CHECK_INSTANCE;
   try
   {
      core->setPosition(stageLabel, position);
   }
   catch (CMMError& e)
   {
      return G2S_ERROR(e);
   }
   return G2S_OK();
}

G2SCLIENTC_API g2s_getPosition(const char* stageLabel, double* position)
{
   CHECK_INSTANCE;
   try
   {
      *position = core->getPosition(stageLabel);
   }
   catch (CMMError& e)
   {
      return G2S_ERROR(e);
   }
   return G2S_OK();
}

/**
 * Returns the name (label) of the current default camera.
 * @param buffer - camera label
 * @param maxLength - maximum buffer length
 */
G2SCLIENTC_API g2s_getCameraDevice(char* buffer, size_t maxLength)
{
   CHECK_INSTANCE;
   try
   {
      string camera = core->getCameraDevice();
      if (camera.size() >= maxLength)
         throw CMMError("Camera name too long to fit in the buffer");
      STRCPYFUNC(buffer, maxLength, camera.c_str());
   }
   catch(CMMError& e)
   {
      return G2S_ERROR(e);
   }
   return G2S_OK();
}

G2SCLIENTC_API g2s_setCameraDevice(const char* buffer)
{
   CHECK_INSTANCE;
   try
   {
      core->setCameraDevice(buffer);
   }
   catch (CMMError& e)
   {
      return G2S_ERROR(e);
   }
   return G2S_OK();
}


/**
 * Starts sequence acquisition on the default camera device.
 * Images are stored in the circular buffer and can be retrieved with
 * getLastImage and popNextImage functions.
 *
 * @param numImages - number of images to acquire
 * @param intervalMs - interval between images in milliseconds
 * @param stopOnOverflow - if true the acquisition will automatically stop if the camera
 * circular buffer overflows. If false, the acquisition will continue until the specified
 * number of images is reached, even if some of the images from the camera are lost due to overflow.
 */
G2SCLIENTC_API g2s_startSequenceAcquisition(long numImages, double intervalMs, g2s_bool stopOnOverflow)
{
   CHECK_INSTANCE;
   try
   {
      core->startSequenceAcquisition(numImages, intervalMs, stopOnOverflow == g2s_true ? true : false);
   }
   catch(CMMError& e)
   {
      return G2S_ERROR(e);
   }
   return G2S_OK();
}
/*
G2SCLIENTC_API g2s_prepareSequenceAcquisition(const char* cameraLabel)
{
   CHECK_INSTANCE;
   try
   {
      core->prepareSequenceAcquisition(cameraLabel);
   }
   catch(CMMError& e)
   {
      return G2S_ERROR(e);
   }
   return G2S_OK();

}
*/

/**
 * Starts the continuous sequence acquisition on the default camera device.
 * The acquisition keeps going until g2s_stopSequenceAcquisition() is called.
 * Images are stored in the circular buffer and can be retrieved with
 * getLastImage and popNextImage functions. The acquisition continues even if 
 * buffer overflows.
 *
 * @param intervalMs - interval between images in milliseconds. This value is just
 * a suggestion to the camera, it is often ignored and the camera just acquires at its
 * maximum speed.
 */

G2SCLIENTC_API g2s_startContinuousSequenceAcquisition(double intervalMs)
{
   CHECK_INSTANCE;
   try
   {
      core->startContinuousSequenceAcquisition(intervalMs);
   }
   catch(CMMError& e)
   {
      return G2S_ERROR(e);
   }
   return G2S_OK();
}

/**
 * Stops sequence acquisition on the default camera. 
 */
G2SCLIENTC_API g2s_stopSequenceAcquisition()
{
   CHECK_INSTANCE;
   try
   {
      core->stopSequenceAcquisition();
   }
   catch(CMMError& e)
   {
      return G2S_ERROR(e);
   }
   return G2S_OK();
}

/**
 * Tells us whether the camera is in the sequence acquisition mode.
 * @param running - true if camera is acquiring
 */
G2SCLIENTC_API g2s_isSequenceRunning(g2s_bool* running)
{
   CHECK_INSTANCE;
   try
   {
      *running = core->isSequenceRunning() ? g2s_true : g2s_false;
   }
   catch(CMMError& e)
   {
      return G2S_ERROR(e);
   }
   return G2S_OK();
}

/**
 * Retrieves the last (freshest) image from the top of the circular buffer, but does not remove it from the queue.
 * The caller must allocate and provide img buffer of adequate size.
 * The size of the current image can be obtained by calling g2s_getImageBufferSize()
 * @param img - image buffer
 * @param bufSize - image buffer size
 */
G2SCLIENTC_API g2s_getLastImage(unsigned char* img, size_t bufSize)
{
   CHECK_INSTANCE;
   try
   {
      memmove(img, core->getLastImage(), bufSize);
   }
   catch(CMMError& e)
   {
      return G2S_ERROR(e);
   }
   return G2S_OK();
}

/**
 * Retrieves the next image queued in the circular buffer, and removes it from the queue.
 * The caller must allocate and provide img buffer of adequate size.
 * The size of the current image can be obtained by calling g2s_getImageBufferSize()
 * @param img - image buffer
 * @param bufSize - image buffer size
 */
G2SCLIENTC_API g2s_popNextImage(unsigned char* img, size_t bufSize)
{
   CHECK_INSTANCE;
   try
   {
      memmove(img, core->popNextImage(), bufSize);
   }
   catch(CMMError& e)
   {
      return G2S_ERROR(e);
   }
   return G2S_OK();
}

/**
 * Retrieves the next image queued in the circular buffer, and removes it from the queue.
 * The caller must allocate and provide img buffer of adequate size.
 * The image is accompanied by metadata stored in ImageInfo structure.
 * The size of the current image can be obtained by calling g2s_getImageBufferSize().
 *
 * @param img - image buffer
 * @param bufSize - image buffer size
 * @param inf - metadata information
 */
G2SCLIENTC_API g2s_getLastImageMD(unsigned char* img, size_t bufSize, g2s_ImageInfo* inf)
{
   CHECK_INSTANCE;
   // TODO
   /*
   try
   {
      std::vector<unsigned char> buf = core->seqGetLastImage(md);
		inf->frameNumber = (unsigned)atoi(md.getString("ImageNumber", "0").c_str());
      memmove(img, &buf[0], bufSize);
   }
   catch(CMMError& e)
   {
      return G2S_ERROR(e);
   }
   */
   return G2S_OK();
}

/**
 * Retrieves the next image queued in the circular buffer, and removes it from the queue.
 * The caller must allocate and provide img buffer of adequate size.
 * The image is accompanied by metadata stored in ImageInfo structure.
 * The size of the current image can be obtained by calling g2s_getImageBufferSize()
 * @param img - image buffer
 * @param bufSize - image buffer size
 * @param inf - metadata information
 */
G2SCLIENTC_API g2s_popNextImageMD(unsigned char* img, size_t bufSize, g2s_ImageInfo* inf)
{
   CHECK_INSTANCE;
   // TODO
   /*
   try
   {
      g2s::G2SConfig md;
      vector<unsigned char> buf = core->seqPopNextImage(md);
		inf->frameNumber = (unsigned)atoi(md.getString("ImageNumber", "0").c_str());
      memmove(img, &buf[0], bufSize);
   }
   catch(CMMError& e)
   {
      return G2S_ERROR(e);
   }
   */
   return G2S_OK();
}

/**
 * Returns the number of images waiting in the circular buffer queue.
 * @param count - number of images
 */
G2SCLIENTC_API g2s_getRemainingImageCount(unsigned* count)
{
   CHECK_INSTANCE;
   *count = core->getRemainingImageCount();
   return G2S_OK();
}

/**
 * Returns the total length of the circular buffer (total capacity)
 * @param capacity - number of images
 */
G2SCLIENTC_API g2s_getBufferTotalCapacity(unsigned* capacity)
{
   CHECK_INSTANCE;
   *capacity = core->getBufferTotalCapacity();
   return G2S_OK();
}

/**
 * Returns the available free size of the circular buffer (free capacity)
 * @param capacity - number of images
 */
G2SCLIENTC_API g2s_getBufferFreeCapacity(unsigned* capacity)
{
   CHECK_INSTANCE;
   *capacity = core->getBufferFreeCapacity();
   return G2S_OK();
}

/**
 * Tells us whether the circular buffer has overflowed.
 * This function does not have sense when using g2s_startContinuousSequenceAcquisition(),
 * because the acquisition continues until stopped, and the buffer gets reset
 * automatically each time it overflows.
 * @param overflowed - true on overflow
 */
G2SCLIENTC_API g2s_isBufferOverflowed(g2s_bool* overflowed)
{
   CHECK_INSTANCE;
   *overflowed = core->isBufferOverflowed() ? g2s_true : g2s_false;
   return G2S_OK();
}

/**
 * Tells us the average interval in milliseconds at which the camera is pumping images into the circular buffer.
 * This is useful if the camera does not report frame rate. In continuous acquisition case this as close as it
 * gets to the camera frame interval.
 * @param intervalMs - average interval in milliseconds
 */
G2SCLIENTC_API g2s_getBufferIntervalMs(double* intervalMs)
{
   CHECK_INSTANCE;
   *intervalMs = 0;
   return G2S_OK();
}

/**
 * Sets the size of the circular buffer in MBytes.
 * @param sizeMB - circular buffer size in MBytes.
 */
G2SCLIENTC_API g2s_setCircularBufferMemoryFootprint(unsigned sizeMB)
{
   CHECK_INSTANCE;
   try
   {
      core->setCircularBufferMemoryFootprint(sizeMB);
   }
   catch(CMMError& e)
   {
      return G2S_ERROR(e);
   }
   return G2S_OK();
}

/*
G2SCLIENTC_API g2s_initializeCircularBuffer()
{
   CHECK_INSTANCE;
   try
   {
      core->initializeCircularBuffer();
   }
   catch(CMMError& e)
   {
      return G2S_ERROR(e);
   }
   return G2S_OK();
}
*/

/**
 * Discards all images waiting in the circular buffer. 
 */
G2SCLIENTC_API g2s_clearCircularBuffer()
{
   CHECK_INSTANCE;
   try
   {
      core->clearCircularBuffer();
   }
   catch(CMMError& e)
   {
      return G2S_ERROR(e);
   }
   return G2S_OK();
}

G2SCLIENTC_API g2s_sleep(double intervalMs)
{
#ifdef WIN32
   Sleep((DWORD) intervalMs);
#else
	sleep((unsigned int)intervalMs);
#endif
   return G2S_OK();
}

#ifdef __cplusplus
};
#endif /* __cplusplus */

