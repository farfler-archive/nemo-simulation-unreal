#pragma once

#include "CoreMinimal.h"
#include "LidarSensor.h"
#include "CameraSensor.h"
#include "Containers/Map.h"

class NEMO_SIMULATOR_API SensorManager
{
public:
	SensorManager();
	~SensorManager();

	void RegisterSensor(ULidarSensor* Lidar);
	void RegisterSensor(UCameraSensor* Camera);

	ULidarSensor* GetLidarSensor(const FString& LidarName) const;
	UCameraSensor* GetCameraSensor(const FString& CameraName) const;

private:
	TMap<FString, ULidarSensor*> RegisteredLidarSensors;
	TMap<FString, UCameraSensor*> RegisteredCameraSensors;
};
