#include "WheelchairSimulator.h"
#include "LidarScanData.h"
#include "CameraImageData.h"
#include "LidarSensor.h"
#include "CameraSensor.h"
#include "NetworkUtils.h"

#define TARGET_FPS 100.0f
#define WHEELCHAIR_SPEED 4.0f

UWheelchairSimulator::UWheelchairSimulator()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UWheelchairSimulator::BeginPlay()
{
	Super::BeginPlay();
	NetworkStreamerFR.InitServer("192.168.100.81", 12345);
	NetworkStreamerFL.InitServer("192.168.100.81", 12346);
	NetworkStreamerRR.InitServer("192.168.100.81", 12347);
	NetworkStreamerRL.InitServer("192.168.100.81", 12348);
	NetworkStreamerCamera.InitServer("192.168.100.81", 12349);

	// Register sensors to the sensor manager
	SetupSensors();
}

void UWheelchairSimulator::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Limit maximum fps
	if (GetWorld()->GetTimeSeconds() - LatestUpdateTime < 1.0f / TARGET_FPS) {
		return;
	}

	NetworkStreamerFR.ListenForConnection();
	NetworkStreamerFL.ListenForConnection();
	NetworkStreamerRR.ListenForConnection();
	NetworkStreamerRL.ListenForConnection();
	NetworkStreamerCamera.ListenForConnection();

	// Create and send Front Right LIDAR scan data
	if (NetworkStreamerFR.IsConnected()) {
		ULidarSensor* LidarFR = SensorManager.GetLidarSensor(TEXT("LIDAR_FR"));
		LidarScanData::LidarScanData ScanData = LidarFR->GetLidarScanData();
		ScanData.frameId = "front_right_laser_frame";
		std::vector<uint8_t> LatestLidarScan = LidarScanData::SerializeLidarScanData(ScanData);
		std::vector<uint8_t> LidarScanPacket = NetworkUtils::CreateDataPacketWithHeader(LatestLidarScan);

		bool Result = NetworkStreamerFR.SendData(LidarScanPacket);
		if (!Result) {
			UE_LOG(LogTemp, Error, TEXT("Failed to send Front Right Lidar scan data"));
		}
	}

	// Create and send Front Left LIDAR scan data
	if (NetworkStreamerFL.IsConnected()) {
		ULidarSensor* LidarFR = SensorManager.GetLidarSensor(TEXT("LIDAR_FL"));
		LidarScanData::LidarScanData ScanData = LidarFR->GetLidarScanData();
		ScanData.frameId = "front_left_laser_frame";
		std::vector<uint8_t> LatestLidarScan = LidarScanData::SerializeLidarScanData(ScanData);
		std::vector<uint8_t> LidarScanPacket = NetworkUtils::CreateDataPacketWithHeader(LatestLidarScan);

		bool Result = NetworkStreamerFL.SendData(LidarScanPacket);
		if (!Result) {
			UE_LOG(LogTemp, Error, TEXT("Failed to send Front Left Lidar scan data"));
		}
	}

	// Create and send Rear Right LIDAR scan data
	if (NetworkStreamerRR.IsConnected()) {
		ULidarSensor* LidarFR = SensorManager.GetLidarSensor(TEXT("LIDAR_RR"));
		LidarScanData::LidarScanData ScanData = LidarFR->GetLidarScanData();
		ScanData.frameId = "rear_right_laser_frame";
		std::vector<uint8_t> LatestLidarScan = LidarScanData::SerializeLidarScanData(ScanData);
		std::vector<uint8_t> LidarScanPacket = NetworkUtils::CreateDataPacketWithHeader(LatestLidarScan);

		bool Result = NetworkStreamerRR.SendData(LidarScanPacket);
		if (!Result) {
			UE_LOG(LogTemp, Error, TEXT("Failed to send Rear Right Lidar scan data"));
		}
	}

	// Create and send Rear Left LIDAR scan data
	if (NetworkStreamerRL.IsConnected()) {
		ULidarSensor* LidarFR = SensorManager.GetLidarSensor(TEXT("LIDAR_RL"));
		LidarScanData::LidarScanData ScanData = LidarFR->GetLidarScanData();
		ScanData.frameId = "rear_left_laser_frame";
		std::vector<uint8_t> LatestLidarScan = LidarScanData::SerializeLidarScanData(ScanData);
		std::vector<uint8_t> LidarScanPacket = NetworkUtils::CreateDataPacketWithHeader(LatestLidarScan);

		bool Result = NetworkStreamerRL.SendData(LidarScanPacket);
		if (!Result) {
			UE_LOG(LogTemp, Error, TEXT("Failed to send Rear Left Lidar scan data"));
		}
	}

	// Create and send camera image data
	if (NetworkStreamerCamera.IsConnected()) {
		UCameraSensor* Camera = SensorManager.GetCameraSensor(TEXT("CAMERA"));
		std::vector<uint8_t> LatestImageData = CameraImageData::SerializeCameraImageData(Camera->GetLatestImage());
		std::vector<uint8_t> ImageDataPacket = NetworkUtils::CreateDataPacketWithHeader(LatestImageData);

		bool Result = NetworkStreamerCamera.SendData(ImageDataPacket);
		if (!Result) {
			UE_LOG(LogTemp, Error, TEXT("Failed to send camera image data"));
		}
	}

	// Move actor forward by WHEELCHAIR_SPEED
	FVector NewLocation = GetOwner()->GetActorLocation();
	NewLocation += GetOwner()->GetActorForwardVector() * WHEELCHAIR_SPEED;
	// GetOwner()->SetActorLocation(NewLocation);
	// UE_LOG(LogTemp, Warning, TEXT("Actor location: %s"), *NewLocation.ToString());

	// Rotate actor by 1 degree
	FRotator NewRotation = GetOwner()->GetActorRotation();
	NewRotation.Yaw += 1.0f;
	// GetOwner()->SetActorRotation(NewRotation);
	// UE_LOG(LogTemp, Warning, TEXT("Actor rotation: %s"), *NewRotation.ToString());

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
		else if (ChildActor->GetName().StartsWith("BP_Camera")) {
			UCameraSensor* Camera = Cast<UCameraSensor>(ChildActor->GetComponentByClass(UCameraSensor::StaticClass()));
			// Register camera sensor
			SensorManager.RegisterSensor(Camera);
		}
	}
}
