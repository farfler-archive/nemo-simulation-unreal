#pragma once

#include "CoreMinimal.h"

namespace CameraImageData
{
	struct CameraImageData
	{
		std::string frameId;
		int32_t stampSec;
		uint32_t stampNanosec;
		uint32_t height;
		uint32_t width;
		std::string encoding;
		uint8_t isBigendian;
		uint32_t step;
		std::vector <uint8_t> data;
	};

	std::vector<uint8_t> SerializeCameraImageData(const CameraImageData& cameraImageData);
}