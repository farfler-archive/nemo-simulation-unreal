#include "WheelchairSimulator.h"
#include "LidarScanData.h"
#include "LidarSensor.h"
#include "NetworkUtils.h"

UWheelchairSimulator::UWheelchairSimulator()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UWheelchairSimulator::BeginPlay()
{
	Super::BeginPlay();
	NetworkStreamer.InitServer("192.168.100.81", 12345);

	// Register sensors to the sensor manager
	SetupSensors();
}

void UWheelchairSimulator::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Limit to maximum 30 fps
	if (GetWorld()->GetTimeSeconds() - LatestUpdateTime < 1.0f / 30.0f) {
		return;
	}

	NetworkStreamer.ListenForConnection();

	ULidarSensor* LidarFR = SensorManager.GetLidarSensor(TEXT("LIDAR_FR"));
	std::vector<uint8_t> LatestLidarScan = LidarScanData::SerializeLidarScanData(LidarFR->GetLatestLidarScanData());

	if (NetworkStreamer.IsConnected()) {
		// Create complete packet with header
		std::vector<uint8_t> LidarScanPacket = NetworkUtils::CreateDataPacketWithHeader(LatestLidarScan);

		bool Result = NetworkStreamer.SendData(LidarScanPacket);
		if (!Result) {
			UE_LOG(LogTemp, Error, TEXT("Failed to send Lidar scan data"));
		}
	}

	LatestUpdateTime = GetWorld()->GetTimeSeconds();
}

void UWheelchairSimulator::SetupSensors()
{
	TArray<AActor*> ChildActors;
	GetOwner()->GetAttachedActors(ChildActors);

	for (AActor* ChildActor : ChildActors) {
		if (ChildActor->GetName().StartsWith("BP_LIDAR")) {
			ULidarSensor* Lidar = Cast<ULidarSensor>(ChildActor->GetComponentByClass(ULidarSensor::StaticClass()));
			// Register LIDAR sensor
			SensorManager.RegisterSensor(Lidar);
		}
	}
}
