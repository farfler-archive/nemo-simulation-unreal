#pragma once

#include "CoreMinimal.h"

namespace LidarScanData
{
	struct LidarScanData
	{
		std::string frameId;
		int32_t stampSec;
		uint32_t stampNanosec;
		float angleMin;
		float angleMax;
		float angleIncrement;
		float timeIncrement;
		float scanTime;
		float rangeMin;
		float rangeMax;
		std::vector <float> ranges;
		std::vector <float> intensities;
	};

	void PrintLidarScanData(const LidarScanData& lidarScanData);
	std::vector<char> SerializeLidarScanData(const LidarScanData& lidarScanData);
}