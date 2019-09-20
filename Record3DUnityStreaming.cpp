#include "include/record3d_unity_streaming/Record3DUnityStreaming.h"
#include <iostream>
#include <record3d/Record3DStructs.h>
#include <record3d/Record3DStream.h>
#include <cmath>


FrameMetadata GetFrameMetadata()
{
	FrameMetadata fmd;
	fmd.width = static_cast<int32_t>(Record3D::Record3DStream::FRAME_WIDTH);
	fmd.height = static_cast<int32_t>(Record3D::Record3DStream::FRAME_HEIGHT);
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


bool StartStreaming(Record3DDevice $deviceHandle, OnNewFrameCallback $newFrameCallback, OnStreamStoppedCallback $streamStoppedCallback)
{
    auto* stream = new Record3D::Record3DStream{};
    constexpr int texWidth = Record3D::Record3DStream::FRAME_WIDTH;
    constexpr int texHeight = Record3D::Record3DStream::FRAME_HEIGHT;
    constexpr int numComponentsPerPointPosition = 4;
    int vertexPositionsBuffLength = texWidth * texHeight * numComponentsPerPointPosition;

    auto* positionsBuffer = new float[vertexPositionsBuffLength];

    stream->onNewFrame = [=](const Record3D::BufferRGB &$rgbFrame,
                             const Record3D::BufferDepth &$depthFrame,
                             uint32_t $frameWidth,
                             uint32_t $frameHeight,
                             Record3D::IntrinsicMatrixCoeffs $K)
    {
        float ifx = 1.0f / $K.fx;
        float ify = 1.0f / $K.fy;
        float itx = -$K.tx / $K.fx;
        float ity = -$K.ty / $K.fy;

        for (int i = 0; i < texHeight; i++)
            for ( int j = 0; j < texWidth; j++ )
            {
                int idx = texWidth * i + j;
                int posBuffIdx = numComponentsPerPointPosition * idx;
                const float currDepth = ((float*)$depthFrame.data())[ idx ];

                positionsBuffer[ posBuffIdx + 0 ] =  (ifx * j + itx) * currDepth;
                positionsBuffer[ posBuffIdx + 1 ] = -(ify * i + ity) * currDepth;
                positionsBuffer[ posBuffIdx + 2 ] = -currDepth;
                positionsBuffer[ posBuffIdx + 3 ] = idx;
            }

		FrameInfo conf;
		conf.depthFrameBufferPtr = (void*)positionsBuffer;
		conf.rgbFrameBufferPtr = (void*)$rgbFrame.data();
		conf.depthFrameBufferSize = static_cast<int32_t>(vertexPositionsBuffLength * sizeof(float));
		conf.rgbFrameBufferSize = static_cast<int32_t>($rgbFrame.size());
		conf.frameWidth = static_cast<int32_t>($frameWidth);
		conf.frameHeight = static_cast<int32_t>($frameHeight);

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