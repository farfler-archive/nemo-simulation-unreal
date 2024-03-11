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
	NewLaserScan.angle_min = -PI;
	NewLaserScan.angle_max = PI;
	NewLaserScan.angle_increment = (LatestLaserScan.angle_max - LatestLaserScan.angle_min) / NumRays;
	NewLaserScan.time_increment = 0.0f; // Assuming the time between scans is negligible
	NewLaserScan.scan_time = 0.0f; // Assuming the scan time is negligible
	NewLaserScan.range_min = MinDistance;
	NewLaserScan.range_max = MaxDistance;

	// Resize the ranges and intensities vectors
	NewLaserScan.ranges.resize(NumRays, 0.0f);
	// NOTE: Do not send intensities, as ROS does not use them
	// NewLaserScan.intensities.resize(NumRays, 0.0f);

	// Trace the rays

	float Angle = 0.0f;
	for (int i = 0; i < NumRays; i++) {
		Angle = i * (360.0f / NumRays);
		float Distance = TraceLine(-Angle); // NOTE: The angle is negated to match the Unreal Engine coordinate system
		NewLaserScan.ranges[i] = Distance;
		// NOTE: Do not send intensities, as ROS does not use them
		// NewLaserScan.intensities[i] = 1.0f;
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

std::vector<char> USensorLIDAR::SerializeLaserScan(const SensorMsgLaserScan& LaserScan)
{
	std::vector<char> buffer;

	buffer.clear();
	size_t offset = 0;

	size_t header_frame_id_size = LaserScan.header_frame_id.size();
	buffer.resize(offset + sizeof(header_frame_id_size));
	memcpy(buffer.data() + offset, &header_frame_id_size, sizeof(header_frame_id_size));
	offset += sizeof(header_frame_id_size);

	buffer.resize(offset + header_frame_id_size);
	memcpy(buffer.data() + offset, LaserScan.header_frame_id.data(), header_frame_id_size);
	offset += header_frame_id_size;

	int32_t header_stamp_sec = LaserScan.header_stamp_sec;
	buffer.resize(offset + sizeof(header_stamp_sec));
	memcpy(buffer.data() + offset, &LaserScan.header_stamp_sec, sizeof(LaserScan.header_stamp_sec));
	offset += sizeof(LaserScan.header_stamp_sec);

	uint32_t header_stamp_nanosec = LaserScan.header_stamp_nanosec;
	buffer.resize(offset + sizeof(header_stamp_nanosec));
	memcpy(buffer.data() + offset, &LaserScan.header_stamp_nanosec, sizeof(LaserScan.header_stamp_nanosec));
	offset += sizeof(LaserScan.header_stamp_nanosec);

	float angle_min = LaserScan.angle_min;
	buffer.resize(offset + sizeof(angle_min));
	memcpy(buffer.data() + offset, &LaserScan.angle_min, sizeof(LaserScan.angle_min));
	offset += sizeof(LaserScan.angle_min);

	float angle_max = LaserScan.angle_max;
	buffer.resize(offset + sizeof(angle_max));
	memcpy(buffer.data() + offset, &LaserScan.angle_max, sizeof(LaserScan.angle_max));
	offset += sizeof(LaserScan.angle_max);

	float angle_increment = LaserScan.angle_increment;
	buffer.resize(offset + sizeof(angle_increment));
	memcpy(buffer.data() + offset, &LaserScan.angle_increment, sizeof(LaserScan.angle_increment));
	offset += sizeof(LaserScan.angle_increment);

	float time_increment = LaserScan.time_increment;
	buffer.resize(offset + sizeof(time_increment));
	memcpy(buffer.data() + offset, &LaserScan.time_increment, sizeof(LaserScan.time_increment));
	offset += sizeof(LaserScan.time_increment);

	float scan_time = LaserScan.scan_time;
	buffer.resize(offset + sizeof(scan_time));
	memcpy(buffer.data() + offset, &LaserScan.scan_time, sizeof(LaserScan.scan_time));
	offset += sizeof(LaserScan.scan_time);

	float range_min = LaserScan.range_min;
	buffer.resize(offset + sizeof(range_min));
	memcpy(buffer.data() + offset, &LaserScan.range_min, sizeof(LaserScan.range_min));
	offset += sizeof(LaserScan.range_min);

	float range_max = LaserScan.range_max;
	buffer.resize(offset + sizeof(range_max));
	memcpy(buffer.data() + offset, &LaserScan.range_max, sizeof(LaserScan.range_max));
	offset += sizeof(LaserScan.range_max);

	size_t ranges_size = LaserScan.ranges.size();
	buffer.resize(offset + sizeof(ranges_size));
	memcpy(buffer.data() + offset, &ranges_size, sizeof(ranges_size));
	offset += sizeof(ranges_size);

	buffer.resize(offset + ranges_size * sizeof(float));
	memcpy(buffer.data() + offset, LaserScan.ranges.data(), ranges_size * sizeof(float));
	offset += ranges_size * sizeof(float);

	size_t intensities_size = LaserScan.intensities.size();
	buffer.resize(offset + sizeof(intensities_size));
	memcpy(buffer.data() + offset, &intensities_size, sizeof(intensities_size));
	offset += sizeof(intensities_size);

	buffer.resize(offset + intensities_size * sizeof(float));
	memcpy(buffer.data() + offset, LaserScan.intensities.data(), intensities_size * sizeof(float));
	offset += intensities_size * sizeof(float);

	return buffer;
}