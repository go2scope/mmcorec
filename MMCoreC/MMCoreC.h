///////////////////////////////////////////////////////////////////////////////
// FILE:          MMCoreC.h
// PROJECT:       MMCoreC
// SUBSYSTEM:     MMCoreC.dll - API
//-----------------------------------------------------------------------------
// DESCRIPTION:   MMCore interface in C, whith no 3rd party dependencies
//                
// AUTHOR:        Nenad Amodaj
// DATE:          08/01/2014 - 2022
// COPYRIGHT:     Luminous Point LLC, http://luminous-point.com
// LICENSE:       Apache License 2.0

#pragma once
#include <stdio.h>

#ifdef WIN32
#ifdef G2SCLIENTC_EXPORTS
#define G2SCLIENTC_API __declspec(dllexport) int
#else
#define G2SCLIENTC_API __declspec(dllimport) int
#endif
#else
#if __GNUC__ >= 4
#define G2SCLIENTC_API __attribute__ ((visibility ("default"))) int
#endif
#endif

#ifdef WIN32
#pragma warning( disable : 4290 )
#pragma warning( disable : 4100 )
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

   #define g2s_MAX_MESSAGE_LENGTH 1024
   #define g2s_OK 0
   #define g2s_INSTANCE_EXISTS 16001
	#define g2s_INVALID_INSTANCE 16001
	#define g2s_SERVER_UNAVAILABLE 16002

   typedef int g2s_bool;
   #define g2s_true 1
   #define g2s_false 0

   typedef struct g2s_ImageInfo
   {
      unsigned frameNumber;
   } g2s_ImageInfo;

   enum g2s_PropertyType {
      Undef = 0,
      String,
      Float,
      Integer
   } g2s_PropertyType;

   enum g2s_DeviceType {
      UnknownType = 0,
      AnyType,
      CameraDevice,
      ShutterDevice,
      StateDevice,
      StageDevice,
      XYStageDevice,
      SerialDevice,
      GenericDevice,
      AutoFocusDevice,
      CoreDevice,
      ImageProcessorDevice,
      SignalIODevice,
      MagnifierDevice,
      SLMDevice,
      HubDevice,
      GalvoDevice
   } g2s_DeviceType;


   // initialization of the MMCC
   G2SCLIENTC_API g2s_create_mmcc();
   G2SCLIENTC_API g2s_delete_mmcc();
   G2SCLIENTC_API g2s_initialize_all_devices();
   G2SCLIENTC_API g2s_initialize_device(const char* label);
   G2SCLIENTC_API g2s_getVersionInfo(char* info, int maxLength);
   G2SCLIENTC_API g2s_getAPIVersionInfo(char* info, int maxLength);
   G2SCLIENTC_API g2s_loadSystemConfiguration(const char* fileName);

   // device setup
   G2SCLIENTC_API g2s_load_device(const char* label, const char* moduleName, const char* deviceName);
   G2SCLIENTC_API g2s_unload_device(const char* label);

   // error handling
   G2SCLIENTC_API g2s_getLastErrorText(char* errMessage, int maxLength);
   G2SCLIENTC_API g2s_getLastErrorCode();
   G2SCLIENTC_API g2s_resetError();

   // general property API
	G2SCLIENTC_API g2s_getNumberOfDeviceProperties(const char* label, size_t* num);
   G2SCLIENTC_API g2s_getDevicePropertyNames(const char* label, char** names, size_t numNames, size_t maxLength);
   G2SCLIENTC_API g2s_getProperty(const char* label, const char* propName, char* value, int maxLength);
   G2SCLIENTC_API g2s_getPropertyFromCache(const char* label, const char* propName, char* value, int maxLength);
   G2SCLIENTC_API g2s_setProperty(const char* label, const char* propName, const char* propValue);
   G2SCLIENTC_API g2s_hasProperty(const char* label, const char* propName, g2s_bool* hasProperty);
	G2SCLIENTC_API g2s_getNumberOfAllowedPropertyValues(const char* label, const char* propName, size_t* num);
	G2SCLIENTC_API g2s_getAllowedPropertyValues(const char* label, const char* propName, char** values, size_t numValues, int maxLength);
   G2SCLIENTC_API g2s_isPropertyReadOnly(const char* label, const char* propName, g2s_bool* readonly);
   G2SCLIENTC_API g2s_hasPropertyLimits(const char* label, const char* propName, g2s_bool* hasLimits);
   G2SCLIENTC_API g2s_getPropertyLowerLimit(const char* label, const char* propName, double* limit);
   G2SCLIENTC_API g2s_getPropertyUpperLimit(const char* label, const char* propName, double* limit);
   G2SCLIENTC_API g2s_getPropertyType(const char* label, const char* propName, enum g2s_PropertyType* type);
   G2SCLIENTC_API g2s_deviceBusy(const char* deviceName, g2s_bool* busy);
   G2SCLIENTC_API g2s_waitForDevice(const char* label);

   // generic device discovery
   G2SCLIENTC_API getLoadedDevices(char** names, size_t numNames, size_t maxLength);
   G2SCLIENTC_API getLoadedDevicesOfType(enum g2s_DeviceType devType);
   G2SCLIENTC_API getDeviceType(const char* label, enum g2s_DeviceType* devType);
   G2SCLIENTC_API getDeviceLibrary(const char* label, char* library, int maxLength);
   G2SCLIENTC_API getDeviceName(const char* label, char* devName, int maxLength);
   G2SCLIENTC_API getDeviceDescription(const char* label, char* descr, int maxLength);

   // Focus (Z) stage control
   G2SCLIENTC_API g2s_getFocusDevice(char* buffer, size_t maxLength);
   G2SCLIENTC_API g2s_setFocusDevice(const char* buffer);

   G2SCLIENTC_API g2s_setPosition(const char* stageLabel, double position);
   G2SCLIENTC_API g2s_getPosition(const char* stageLabel, double* position);

   // Camera API
   G2SCLIENTC_API g2s_getCameraDevice(char* buffer, size_t maxLength);
   G2SCLIENTC_API g2s_setCameraDevice(const char* buffer);

   G2SCLIENTC_API g2s_setExposure(double exp);
   G2SCLIENTC_API g2s_getExposure(double* exposure);
   G2SCLIENTC_API g2s_getImage(unsigned char* img, size_t bufSize);
	G2SCLIENTC_API g2s_getImageBufferSize(size_t* bufSize);
   G2SCLIENTC_API g2s_snapImage();
   G2SCLIENTC_API g2s_getImageWidth(unsigned* w);
   G2SCLIENTC_API g2s_getImageHeight(unsigned* h);
   G2SCLIENTC_API g2s_getBytesPerPixel(unsigned* byteDepth);
   G2SCLIENTC_API g2s_getImageBitDepth(unsigned* bitDepth);
   G2SCLIENTC_API g2s_getNumberOfComponents(unsigned* numComp);
   G2SCLIENTC_API g2s_getNumberOfCameraChannels(unsigned* numChan);
   G2SCLIENTC_API g2s_getCameraChannelName(unsigned chan, char* name, size_t maxLength);

   // image sequence
   G2SCLIENTC_API g2s_startSequenceAcquisition(long numImages, double intervalMs, g2s_bool stopOnOverflow);
   G2SCLIENTC_API g2s_startContinuousSequenceAcquisition(double intervalMs);
   G2SCLIENTC_API g2s_stopSequenceAcquisition();
   G2SCLIENTC_API g2s_isSequenceRunning(g2s_bool* running);
   G2SCLIENTC_API g2s_getLastImage(unsigned char* img, size_t bufSize);
   G2SCLIENTC_API g2s_popNextImage(unsigned char* img, size_t bufSize);
   G2SCLIENTC_API g2s_getLastImageMD(unsigned char* img, size_t bufSize, struct g2s_ImageInfo* inf);
   G2SCLIENTC_API g2s_popNextImageMD(unsigned char* img, size_t bufSize, struct g2s_ImageInfo* inf);

   // camera buffer
   G2SCLIENTC_API g2s_getRemainingImageCount(unsigned* count);
   G2SCLIENTC_API g2s_getBufferTotalCapacity(unsigned* capacity);
   G2SCLIENTC_API g2s_getBufferFreeCapacity(unsigned* capacity);
   G2SCLIENTC_API g2s_getBufferIntervalMs(double* intervalMs);
   G2SCLIENTC_API g2s_isBufferOverflowed(g2s_bool* overflowed);
   G2SCLIENTC_API g2s_setCircularBufferMemoryFootprint(unsigned sizeMB);
   G2SCLIENTC_API g2s_clearCircularBuffer();

   // utility
   G2SCLIENTC_API g2s_sleep(double intervalMs);

#ifdef __cplusplus
};
#endif /* __cplusplus */

// function prototypes for dynamic loading of the MMCoreC
typedef int (*fn_create_mmcc)();
typedef int (*fn_delete_mmcc)();
typedef int (*fn_initialize_all_devices)();
typedef int (*fn_initialize_device)(const char*);

typedef int (*fn_load_device)(const char*, const char*, const char*);
typedef int (*fn_unload_device)(const char*);

typedef int (*fn_get_version_info)(char*, int);
typedef int (*fn_get_api_version_info)(char*, int);
typedef int (*fn_get_last_error_text)(char*, int);

typedef int (*fn_set_position)(const char*, double);
typedef int (*fn_get_position)(const char*, double*);

typedef int (*fn_device_busy)(const char*, int*);

typedef int (*fn_get_property)(const char*, const char*, char*, int);
typedef int (*fn_set_property)(const char*, const char*, const char*, int);

