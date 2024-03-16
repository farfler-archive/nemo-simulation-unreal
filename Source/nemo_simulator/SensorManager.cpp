#include "SensorManager.h"

SensorManager::SensorManager()
{
}

SensorManager::~SensorManager()
{
	RegisteredLidarSensors.Empty();
}

void SensorManager::RegisterSensor(ULidarSensor* Lidar)
{
	if (Lidar)
	{
		FString LidarName = Lidar->LidarName;
		RegisteredLidarSensors.Add(LidarName, Lidar);

		UE_LOG(LogTemp, Warning, TEXT("Registered Lidar Sensor: %s"), *LidarName);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Lidar Sensor is NULL"));
	}
}

ULidarSensor* SensorManager::GetLidarSensor(const FString& LidarName) const
{
	ULidarSensor* Lidar = RegisteredLidarSensors.FindRef(LidarName);
	if (Lidar)
	{
		return Lidar;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Lidar Sensor not found: %s"), *LidarName);
		return nullptr;
	}
}
