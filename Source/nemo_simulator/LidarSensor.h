#pragma once

#include "LidarScanData.h"

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LidarSensor.generated.h"


// ULidarSensor Class Declaration
// Represents a Lidar Sensor component in the simulation environment.
// Responsible for simulation Lidar sensor data collection in a virtual environment.
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NEMO_SIMULATOR_API ULidarSensor : public UActorComponent
{
	GENERATED_BODY()

public:
	ULidarSensor(); // Constructor

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	// Lidar sensor parameter settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lidar Sensor")
	FString LidarName = "LIDAR"; // Name of the Lidar sensor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lidar Sensor")
	float MaxRange = 1000.0f; // Maximum sensing range of the Lidar in units
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lidar Sensor")
	float MinRange = 0.0f; // Minimum sensing range of the Lidar in units
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lidar Sensor")
	int NumLidarRays = 5000; // Number of rays to simulate Lidar scanning
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lidar Sensor")
	bool bDrawRay = false; // Whether to visualize the Lidar rays in the simulation environment
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lidar Sensor")
	FColor RayColor = FColor::Green; // Color of the Lidar rays in the simulation environment

	// Returns the most recent Lidar scan data collected by the sensor
	LidarScanData::LidarScanData GetLidarScanData();

private:
	LidarScanData::LidarScanData LatestLidarScanData; // Stores the latest Lidar scan data

	void UpdateLidarScan(); // Updates the Lidar scan data by simulating a complete scan

	// Casts a Lidar ray from the sensor and returns the distance to the first obstacle hit
	float CastLidarRay(const FVector& Start, const FRotator& OwnerRotation, float Angle);
};
