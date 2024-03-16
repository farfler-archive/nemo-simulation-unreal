#pragma once

#include "CoreMinimal.h"
#include "LidarSensor.h"
#include "Containers/Map.h"

class NEMO_SIMULATOR_API SensorManager
{
public:
	SensorManager();
	~SensorManager();

	void RegisterSensor(ULidarSensor* Lidar);

	ULidarSensor* GetLidarSensor(const FString& LidarName) const;

private:
	TMap<FString, ULidarSensor*> RegisteredLidarSensors;
};
