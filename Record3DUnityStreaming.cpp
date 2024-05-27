#include "include/record3d_unity_streaming/Record3DUnityStreaming.h"
#include <iostream>
#include <record3d/Record3DStructs.h>
#include <record3d/Record3DStream.h>
#include <cmath>


FrameMetadata GetFrameMetadata()
{
	FrameMetadata fmd;
	fmd.numComponentsPerPositionTexturePixel = 4;
	fmd.numComponentsPerColorTexturePixel = 3;

    return fmd;
}


DeviceHandlesInfo ListAllDeviceHandles()
{
    auto connectedDevices = Record3D::Record3DStream::GetConnectedDevices();
    auto* connectedDevicesHandles = (int32_t*) malloc(connectedDevices.size() * sizeof(int32_t));

    for (int i = 0; i < connectedDevices.size(); i++)
    {
        connectedDevicesHandles[i] = static_cast<int32_t>(connectedDevices[i].handle);
    }

	DeviceHandlesInfo info;
	info.arr_ptr = connectedDevicesHandles;
	info.arr_size = static_cast<uint32_t>(connectedDevices.size());

    return info;
}


void FinishDeviceInfoHandling(DeviceHandlesInfo $devInfo)
{
    free($devInfo.arr_ptr);
}

static float InterpolateDepth(const float* $depthData, float $x, float $y, int $imgWidth, int $imgHeight)
{
    int wX = static_cast<int>($x);
    int wY = static_cast<int>($y);
    float fracX = $x - static_cast<float>(wX);
    float fracY = $y - static_cast<float>(wY);

    int topLeftIdx = wY * $imgWidth + wX;
    int topRightIdx = wY * $imgWidth + std::min(wX, $imgWidth - 1);
    int bottomLeftIdx = std::min(wY, $imgHeight - 1) * $imgWidth + wX;
    int bottomRightIdx = std::min(wY, $imgHeight - 1) * $imgWidth + std::min(wX, $imgWidth - 1);

    float interpVal =
        ($depthData[topLeftIdx]    * (1.0f - fracX) + fracX * $depthData[topRightIdx])    * (1.0f - fracY) +
        ($depthData[bottomLeftIdx] * (1.0f - fracX) + fracX * $depthData[bottomRightIdx]) * fracY;

    return interpVal;
}

bool StartStreaming(Record3DDevice $deviceHandle, OnNewFrameCallback $newFrameCallback, OnStreamStoppedCallback $streamStoppedCallback)
{
    auto* stream = new Record3D::Record3DStream{};
    constexpr int numComponentsPerPointPosition = 4;

    size_t positionsBufferSize = 0;
    float* positionsBuffer = nullptr;

    stream->onNewFrame = [=](const Record3D::BufferRGB &$rgbFrame,
                             const Record3D::BufferDepth &$depthFrame,
                             const Record3D::BufferConfidence &$confFrame,
                             const Record3D::BufferMisc &$miscData,
                             uint32_t   $rgbWidth,
                             uint32_t   $rgbHeight,
                             uint32_t   $depthWidth,
                             uint32_t   $depthHeight,
                             uint32_t   $confWidth,
                             uint32_t   $confHeight,
                             Record3D::DeviceType $deviceType,
                             Record3D::IntrinsicMatrixCoeffs $K,
                             Record3D::CameraPose $cameraPose ) mutable
    {
        size_t currPositionsBufferSize = $rgbWidth * $rgbHeight * numComponentsPerPointPosition;
        if ( positionsBufferSize < currPositionsBufferSize )
        {
            if ( positionsBuffer != nullptr )
            {
                delete[] positionsBuffer;
            }

            positionsBuffer = new float[currPositionsBufferSize];

            positionsBufferSize = currPositionsBufferSize;
        }

        float ifx = 1.0f / $K.fx;
        float ify = 1.0f / $K.fy;
        float itx = -$K.tx / $K.fx;
        float ity = -$K.ty / $K.fy;

        bool needToInterpolate = $rgbWidth != $depthWidth || $rgbHeight != $depthHeight;

        float invRGBWidth = 1.0f / $rgbWidth;
        float invRGBHeight = 1.0f / $rgbHeight;

        const float* depthDataPtr = (float*) $depthFrame.data();

        for (int i = 0; i < $rgbHeight; i++)
            for ( int j = 0; j < $rgbWidth; j++ )
            {
                int idx = $rgbWidth * i + j;
                int posBuffIdx = numComponentsPerPointPosition * idx;
                float depthX = invRGBWidth * $depthWidth * j;
                float depthY = invRGBHeight * $depthHeight * i;
                const float currDepth = needToInterpolate ? InterpolateDepth(depthDataPtr, depthX, depthY, $depthWidth, $depthHeight) : depthDataPtr[ idx ];

                positionsBuffer[ posBuffIdx + 0 ] =  (ifx * j + itx) * currDepth;
                positionsBuffer[ posBuffIdx + 1 ] = -(ify * i + ity) * currDepth;
                positionsBuffer[ posBuffIdx + 2 ] = -currDepth;
                positionsBuffer[ posBuffIdx + 3 ] = idx;
            }

        constexpr int numRGBChannels = 3;
		FrameInfo conf;
		conf.depthFrameBufferPtr = (void*)positionsBuffer;
		conf.rgbFrameBufferPtr = (void*)$rgbFrame.data();
		conf.depthFrameBufferSize = static_cast<int32_t>($depthWidth * $depthHeight * sizeof(float) * 4);
		conf.rgbFrameBufferSize = static_cast<int32_t>($rgbWidth * $rgbHeight * numRGBChannels * sizeof(uint8_t));
		conf.frameWidth = static_cast<int32_t>($rgbWidth);
		conf.frameHeight = static_cast<int32_t>($rgbHeight);

        $newFrameCallback(conf);
    };

    stream->onStreamStopped = [=]() {
        $streamStoppedCallback();
        delete stream;
        delete[] positionsBuffer;
    };

	Record3D::DeviceInfo dev;
	dev.handle = static_cast<uint32_t>($deviceHandle.handle);
	dev.udid = "";
	dev.productId = 0;

    bool connectionEstablished = stream->ConnectToDevice(dev);
    return connectionEstablished;
}