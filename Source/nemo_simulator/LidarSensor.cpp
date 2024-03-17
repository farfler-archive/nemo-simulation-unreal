#include "LidarSensor.h"
#include "LidarScanData.h"

ULidarSensor::ULidarSensor()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void ULidarSensor::BeginPlay()
{
	Super::BeginPlay();
}

void ULidarSensor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateLidarScan(); // Update the Lidar scan data

	// LidarScanData::PrintLidarScanData(LatestLidarScanData);	// Print latest Lidar scan data
}

LidarScanData::LidarScanData ULidarSensor::GetLatestLidarScanData()
{
	return LatestLidarScanData;
}

// Updates the Lidar scan data by simulating a complete scan
void ULidarSensor::UpdateLidarScan()
{
	LidarScanData::LidarScanData ScanData;

	// Setup basic Lidar scan data parameters
	ScanData.frameId = "LIDAR"; // TCHAR_TO_UTF8(*LidarName);
	ScanData.stampSec = static_cast<uint32_t>(GetWorld()->GetTimeSeconds());
	ScanData.stampNanosec = static_cast<uint32_t>((GetWorld()->GetTimeSeconds() - ScanData.stampSec) * 1e9);
	ScanData.angleMin = -PI;
	ScanData.angleMax = PI;
	ScanData.angleIncrement = 2 * PI / NumLidarRays;
	ScanData.timeIncrement = 0.0;
	ScanData.scanTime = 0.0;
	ScanData.rangeMin = MinRange;
	ScanData.rangeMax = MaxRange;

	ScanData.ranges = std::vector<float>(NumLidarRays, 0.0f); // Initialize the ranges array with zeros
	ScanData.intensities = std::vector<float>(NumLidarRays, 1.0f); // Initialize the intensities array with ones

	const FVector SensorLocation = GetOwner()->GetActorLocation(); // Sensor's location
	const FRotator SensorRotation = GetOwner()->GetActorRotation(); // Sensor's orientation

	// Simulate Lidar rays around the sensor
	for (int RayIndex = 0; RayIndex < NumLidarRays; RayIndex++) {
		float AngleInDegrees = RayIndex * 360.0f / NumLidarRays; // Angle for the current ray
		AngleInDegrees = 360.0f - AngleInDegrees; // Reverse the angle to match ROS's coordinate system
		// Cast a ray and store the distance to the hit point
		float RayDistance = CastLidarRay(SensorLocation, SensorRotation, AngleInDegrees);
		ScanData.ranges[RayIndex] = RayDistance; // Store the distance in the ranges array
	}

	// Update the latest Lidar scan data
	LatestLidarScanData = ScanData;
}

float ULidarSensor::CastLidarRay(const FVector& Start, const FRotator& OwnerRotation, float AngleInDegrees)
{
	// Convert angle to radians and calculate the end point of the ray
	const float AngleInRadians = FMath::DegreesToRadians(AngleInDegrees);
	const FVector EndOffset = FVector(MaxRange * FMath::Cos(AngleInRadians), MaxRange * FMath::Sin(AngleInRadians), 0.0f);
	const FVector End = Start + OwnerRotation.RotateVector(EndOffset);

	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(GetOwner()); // Ignore the actor that owns the sensor

	// Perform the raycast
	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility, CollisionParams);

	if (HitResult.bBlockingHit) {
		if (bDrawRay) {
			// Draw a line from the sensor to the hit point and mark the hit point
			DrawDebugLine(GetWorld(), Start, HitResult.ImpactPoint, FColor::Green, false, -1.0f, 0, 0.5f);
			DrawDebugPoint(GetWorld(), HitResult.ImpactPoint, 10, FColor::Green, false, -1.0f, 0);
		}
		return HitResult.Distance; // Return the distance to the hit point
	}
	else {
		if (bDrawRay) {
			// Draw a line from the sensor to the end of the ray
			DrawDebugLine(GetWorld(), Start, End, RayColor, false, -1.0f, 0, 0.5f);
		}
		return 0; // Return 0 if the ray didn't hit anything
	}
}