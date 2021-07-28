#ifndef RECORD3D_UNITY_STREAMING_H
#define RECORD3D_UNITY_STREAMING_H

#include <string>
#include <vector>
#include <record3d/Record3DStructs.h>
#include <iostream>

#ifdef __cplusplus
#ifdef WIN32
#define EXPORT_DLL __declspec(dllexport)
extern "C"
{
#else
#define EXPORT_DLL
extern "C"
{
#endif
#endif

    struct DeviceHandlesInfo
    {
        void* arr_ptr;
        uint32_t arr_size;
    };

    struct FrameMetadata
    {
        int32_t numComponentsPerPositionTexturePixel;
        int32_t numComponentsPerColorTexturePixel;
    };

    struct FrameInfo
    {
        // Frame
        void* depthFrameBufferPtr;
        void* rgbFrameBufferPtr;
        int32_t depthFrameBufferSize;
        int32_t rgbFrameBufferSize;
        int32_t frameWidth;
        int32_t frameHeight;
    };

    struct Record3DDevice
    {
        int32_t handle;
    };

	EXPORT_DLL FrameMetadata GetFrameMetadata();
	EXPORT_DLL DeviceHandlesInfo ListAllDeviceHandles();
	EXPORT_DLL void FinishDeviceInfoHandling(DeviceHandlesInfo $devInfo);

    typedef void (*OnNewFrameCallback)(FrameInfo);
    typedef void (*OnStreamStoppedCallback)();
	EXPORT_DLL bool StartStreaming(Record3DDevice $deviceHandle, OnNewFrameCallback $newFrameCallback, OnStreamStoppedCallback $streamStoppedCallback);

#ifdef __cplusplus
}
#endif

#endif //RECORD3D_UNITY_STREAMING_H
