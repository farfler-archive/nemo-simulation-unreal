// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SensorLIDAR.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NEMO_SIMULATOR_API USensorLIDAR : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USensorLIDAR();

	struct SensorMsgLaserScan
	{
		std::string header_frame_id;
		int32_t header_stamp_sec;
		uint32_t header_stamp_nanosec;

		float angle_min;
		float angle_max;
		float angle_increment;
		float time_increment;
		float scan_time;
		float range_min;
		float range_max;

		std::vector<float> ranges;
		std::vector<float> intensities;
	};

	SensorMsgLaserScan GetLatestLaserScan();
	void PrintLaserScan(SensorMsgLaserScan LaserScan);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAcces = "true")) float MaxDistance = 1000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAcces = "true")) float MinDistance = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAcces = "true")) bool bShouldDrawTrace = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAcces = "true")) FString SensorName = "LIDAR";
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAcces = "true")) FColor RayColor = FColor::Red;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAcces = "true")) float RayThickness = 0.2f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAcces = "true")) int NumRays = 360;

private:
	float TraceLine(float Angle);

	SensorMsgLaserScan LatestLaserScan;
		
};
