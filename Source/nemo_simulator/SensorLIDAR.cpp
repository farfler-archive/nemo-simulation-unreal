// Fill out your copyright notice in the Description page of Project Settings.


#include "SensorLIDAR.h"

// Sets default values for this component's properties
USensorLIDAR::USensorLIDAR()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USensorLIDAR::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void USensorLIDAR::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	SensorMsgLaserScan NewLaserScan = {};

	// Fill in the header information
	NewLaserScan.header_frame_id = std::string(TCHAR_TO_UTF8(*SensorName));
	NewLaserScan.header_stamp_sec = static_cast<int32_t>(GetWorld()->GetTimeSeconds());
	NewLaserScan.header_stamp_nanosec = static_cast<uint32_t>((GetWorld()->GetTimeSeconds() - LatestLaserScan.header_stamp_sec) * 1e9);

	// Set the scan parameters
	NewLaserScan.angle_min = 0.0f;
	NewLaserScan.angle_max = 360.0f * PI / 180.0f;
	NewLaserScan.angle_increment = (LatestLaserScan.angle_max - LatestLaserScan.angle_min) / NumRays;
	NewLaserScan.time_increment = 0.0f; // Assuming the time between scans is negligible
	NewLaserScan.scan_time = 0.0f; // Assuming the scan time is negligible
	NewLaserScan.range_min = MinDistance;
	NewLaserScan.range_max = MaxDistance;

	// Resize the ranges and intensities vectors
	NewLaserScan.ranges.resize(NumRays, 0.0f);
	NewLaserScan.intensities.resize(NumRays, 0.0f);

	// Trace the rays

	float Angle = 0.0f;
	for (int i = 0; i < NumRays; i++) {
		Angle = i * (360.0f / NumRays);
		float Distance = TraceLine(Angle);
		NewLaserScan.ranges[i] = Distance;
		NewLaserScan.intensities[i] = 1.0f;
	}

	// Update the latest laser scan
	LatestLaserScan = NewLaserScan;

	// Print the latest laser scan
	// PrintLaserScan(LatestLaserScan);

}

float USensorLIDAR::TraceLine(float Angle)
{
	FVector Start = GetOwner()->GetActorLocation();

	FRotator OwnerRotation = GetOwner()->GetActorRotation();
	FVector EndOffset = FVector(MaxDistance * FMath::Cos(FMath::DegreesToRadians(Angle)), MaxDistance * FMath::Sin(FMath::DegreesToRadians(Angle)), 0.0f);
	FVector End = Start + OwnerRotation.RotateVector(EndOffset);

	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(GetOwner());

	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility, CollisionParams);

	if (HitResult.bBlockingHit) {
		if (bShouldDrawTrace) {
			DrawDebugLine(GetWorld(), Start, HitResult.ImpactPoint, FColor::Green, false, -1.0f, 0, RayThickness);
			DrawDebugPoint(GetWorld(), HitResult.ImpactPoint, 10, FColor::Green, false, -1.0f, 0);
		}
		return HitResult.Distance;
	}
	else {
		if (bShouldDrawTrace) {
			DrawDebugLine(GetWorld(), Start, End, RayColor, false, -1.0f, 0, RayThickness);
		}
		return 0;
	}
}

USensorLIDAR::SensorMsgLaserScan USensorLIDAR::GetLatestLaserScan()
{
	return LatestLaserScan;
}

void USensorLIDAR::PrintLaserScan(SensorMsgLaserScan LaserScan)
{
	UE_LOG(LogTemp, Warning, TEXT("LatestLaserScan.header_frame_id: %s"), *FString(LatestLaserScan.header_frame_id.c_str()));
	UE_LOG(LogTemp, Warning, TEXT("LatestLaserScan.header_stamp_sec: %d"), LatestLaserScan.header_stamp_sec);
	UE_LOG(LogTemp, Warning, TEXT("LatestLaserScan.header_stamp_nanosec: %d"), LatestLaserScan.header_stamp_nanosec);
	UE_LOG(LogTemp, Warning, TEXT("LatestLaserScan.angle_min: %f"), LatestLaserScan.angle_min);
	UE_LOG(LogTemp, Warning, TEXT("LatestLaserScan.angle_max: %f"), LatestLaserScan.angle_max);
	UE_LOG(LogTemp, Warning, TEXT("LatestLaserScan.angle_increment: %f"), LatestLaserScan.angle_increment);
	UE_LOG(LogTemp, Warning, TEXT("LatestLaserScan.time_increment: %f"), LatestLaserScan.time_increment);
	UE_LOG(LogTemp, Warning, TEXT("LatestLaserScan.scan_time: %f"), LatestLaserScan.scan_time);
	UE_LOG(LogTemp, Warning, TEXT("LatestLaserScan.range_min: %f"), LatestLaserScan.range_min);
	UE_LOG(LogTemp, Warning, TEXT("LatestLaserScan.range_max: %f"), LatestLaserScan.range_max);
	UE_LOG(LogTemp, Warning, TEXT("LatestLaserScan.ranges.size(): %d"), LatestLaserScan.ranges.size());
	UE_LOG(LogTemp, Warning, TEXT("LatestLaserScan.intensities.size(): %d"), LatestLaserScan.intensities.size());
	UE_LOG(LogTemp, Warning, TEXT("LatestLaserScan.ranges[0]: %f"), LatestLaserScan.ranges[0]);
	UE_LOG(LogTemp, Warning, TEXT("LatestLaserScan.intensities[0]: %f"), LatestLaserScan.intensities[0]);
	UE_LOG(LogTemp, Warning, TEXT("LatestLaserScan.ranges[NumRays - 1]: %f"), LatestLaserScan.ranges[NumRays - 1]);
	UE_LOG(LogTemp, Warning, TEXT("LatestLaserScan.intensities[NumRays - 1]: %f"), LatestLaserScan.intensities[NumRays - 1]);
}