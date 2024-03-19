#include "SensorManager.h"

SensorManager::SensorManager()
{
}

SensorManager::~SensorManager()
{
	RegisteredLidarSensors.Empty();
	RegisteredCameraSensors.Empty();
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

void SensorManager::RegisterSensor(UCameraSensor* Camera)
{
	if (Camera)
	{
		FString CameraName = Camera->CameraName;
		RegisteredCameraSensors.Add(CameraName, Camera);

		UE_LOG(LogTemp, Warning, TEXT("Registered Camera Sensor: %s"), *CameraName);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Camera Sensor is NULL"));
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

UCameraSensor* SensorManager::GetCameraSensor(const FString& CameraName) const
{
	UCameraSensor* Camera = RegisteredCameraSensors.FindRef(CameraName);
	if (Camera)
	{
		return Camera;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Camera Sensor not found: %s"), *CameraName);
		return nullptr;
	}
}