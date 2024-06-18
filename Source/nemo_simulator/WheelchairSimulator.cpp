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
	NetworkStreamerFR.InitServer("192.168.100.81", 42071);
	NetworkStreamerFL.InitServer("192.168.100.81", 42070);
	NetworkStreamerRR.InitServer("192.168.100.81", 42073);
	NetworkStreamerRL.InitServer("192.168.100.81", 42072);
	NetworkStreamerCamera.InitServer("192.168.100.81", 12349);
	NetworkStreamerVelocity.InitServer("192.168.100.81", 42069);

	// Register sensors to the sensor manager
	SetupSensors();
}

void UWheelchairSimulator::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Limit maximum fps
	if (GetWorld()->GetTimeSeconds() - LatestUpdateTime < 1.0f / TARGET_FPS)
	{
		return;
	}

	NetworkStreamerFR.ListenForConnection();
	NetworkStreamerFL.ListenForConnection();
	NetworkStreamerRR.ListenForConnection();
	NetworkStreamerRL.ListenForConnection();
	NetworkStreamerCamera.ListenForConnection();
	NetworkStreamerVelocity.ListenForConnection();

	// Create and send Front Right LIDAR scan data
	if (NetworkStreamerFR.IsConnected())
	{
		ULidarSensor *LidarFR = SensorManager.GetLidarSensor(TEXT("LIDAR_FR"));
		LidarScanData::LidarScanData ScanData = LidarFR->GetLidarScanData();
		ScanData.frameId = "front_right_laser_frame";
		std::vector<uint8_t> LatestLidarScan = LidarScanData::SerializeLidarScanData(ScanData);
		std::vector<uint8_t> LidarScanPacket = NetworkUtils::CreateDataPacketWithHeader(LatestLidarScan);

		bool Result = NetworkStreamerFR.SendData(LidarScanPacket);
		if (!Result)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to send Front Right Lidar scan data"));
		}
	}

	// Create and send Front Left LIDAR scan data
	if (NetworkStreamerFL.IsConnected())
	{
		ULidarSensor *LidarFR = SensorManager.GetLidarSensor(TEXT("LIDAR_FL"));
		LidarScanData::LidarScanData ScanData = LidarFR->GetLidarScanData();
		ScanData.frameId = "front_left_laser_frame";
		std::vector<uint8_t> LatestLidarScan = LidarScanData::SerializeLidarScanData(ScanData);
		std::vector<uint8_t> LidarScanPacket = NetworkUtils::CreateDataPacketWithHeader(LatestLidarScan);

		bool Result = NetworkStreamerFL.SendData(LidarScanPacket);
		if (!Result)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to send Front Left Lidar scan data"));
		}
	}

	// Create and send Rear Right LIDAR scan data
	if (NetworkStreamerRR.IsConnected())
	{
		ULidarSensor *LidarFR = SensorManager.GetLidarSensor(TEXT("LIDAR_RR"));
		LidarScanData::LidarScanData ScanData = LidarFR->GetLidarScanData();
		ScanData.frameId = "rear_right_laser_frame";
		std::vector<uint8_t> LatestLidarScan = LidarScanData::SerializeLidarScanData(ScanData);
		std::vector<uint8_t> LidarScanPacket = NetworkUtils::CreateDataPacketWithHeader(LatestLidarScan);

		bool Result = NetworkStreamerRR.SendData(LidarScanPacket);
		if (!Result)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to send Rear Right Lidar scan data"));
		}
	}

	// Create and send Rear Left LIDAR scan data
	if (NetworkStreamerRL.IsConnected())
	{
		ULidarSensor *LidarFR = SensorManager.GetLidarSensor(TEXT("LIDAR_RL"));
		LidarScanData::LidarScanData ScanData = LidarFR->GetLidarScanData();
		ScanData.frameId = "rear_left_laser_frame";
		std::vector<uint8_t> LatestLidarScan = LidarScanData::SerializeLidarScanData(ScanData);
		std::vector<uint8_t> LidarScanPacket = NetworkUtils::CreateDataPacketWithHeader(LatestLidarScan);

		bool Result = NetworkStreamerRL.SendData(LidarScanPacket);
		if (!Result)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to send Rear Left Lidar scan data"));
		}
	}

	// Create and send camera image data
	if (NetworkStreamerCamera.IsConnected())
	{
		UCameraSensor *Camera = SensorManager.GetCameraSensor(TEXT("CAMERA"));
		std::vector<uint8_t> LatestImageData = CameraImageData::SerializeCameraImageData(Camera->GetLatestImage());
		std::vector<uint8_t> ImageDataPacket = NetworkUtils::CreateDataPacketWithHeader(LatestImageData);

		bool Result = NetworkStreamerCamera.SendData(ImageDataPacket);
		if (!Result)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to send camera image data"));
		}
	}

	if (NetworkStreamerVelocity.IsConnected())
	{
		std::vector<uint8_t> data = NetworkStreamerVelocity.ReceiveData();

		// Check if any data was received
		if (data.size() != 0)
		{
			if (data.size() % 52 != 0)
			{
				UE_LOG(LogTemp, Error, TEXT("Received invalid message size: %d"), data.size());
			}
			else
			{
				// Print message size
				UE_LOG(LogTemp, Warning, TEXT("Received message size: %d"), data.size());
				int numMessages = data.size() / 52 - 1;
				int messageOffset = numMessages * (6 * sizeof(double) + sizeof(uint32_t)) + sizeof(uint32_t);
				// Parse message
				double last_lin_x = 0.0;
				memcpy(&last_lin_x, &data[messageOffset + 0 * sizeof(double)], sizeof(double));
				UE_LOG(LogTemp, Warning, TEXT("Last lin_x: %f"), last_lin_x);

				double last_lin_y = 0.0;
				memcpy(&last_lin_y, &data[messageOffset + 1 * sizeof(double)], sizeof(double));
				UE_LOG(LogTemp, Warning, TEXT("Last lin_y: %f"), last_lin_y);

				double last_lin_z = 0.0;
				memcpy(&last_lin_z, &data[messageOffset + 2 * sizeof(double)], sizeof(double));
				UE_LOG(LogTemp, Warning, TEXT("Last lin_z: %f"), last_lin_z);

				double last_ang_x = 0.0;
				memcpy(&last_ang_x, &data[messageOffset + 3 * sizeof(double)], sizeof(double));
				UE_LOG(LogTemp, Warning, TEXT("Last ang_x: %f"), last_ang_x);

				double last_ang_y = 0.0;
				memcpy(&last_ang_y, &data[messageOffset + 4 * sizeof(double)], sizeof(double));
				UE_LOG(LogTemp, Warning, TEXT("Last ang_y: %f"), last_ang_y);

				double last_ang_z = 0.0;
				memcpy(&last_ang_z, &data[messageOffset + 5 * sizeof(double)], sizeof(double));
				UE_LOG(LogTemp, Warning, TEXT("Last ang_z: %f"), last_ang_z);
		
				// Move actor forward by WHEELCHAIR_SPEED
				FVector NewLocation = GetOwner()->GetActorLocation();
				NewLocation += GetOwner()->GetActorForwardVector() * WHEELCHAIR_SPEED * last_lin_x;
				GetOwner()->SetActorLocation(NewLocation);
				// UE_LOG(LogTemp, Warning, TEXT("Actor location: %s"), *NewLocation.ToString());

				// Rotate actor by last_ang_z
				FRotator NewRotation = GetOwner()->GetActorRotation();
				NewRotation.Yaw -= last_ang_z;
				GetOwner()->SetActorRotation(NewRotation);
				// UE_LOG(LogTemp, Warning, TEXT("Actor rotation: %s"), *NewRotation.ToString());
			}
		}
	}

	LatestUpdateTime = GetWorld()->GetTimeSeconds();
}

void UWheelchairSimulator::SetupSensors()
{
	TArray<AActor *> ChildActors;
	GetOwner()->GetAttachedActors(ChildActors);

	for (AActor *ChildActor : ChildActors)
	{
		if (ChildActor->GetName().StartsWith("BP_LIDAR"))
		{
			ULidarSensor *Lidar = Cast<ULidarSensor>(ChildActor->GetComponentByClass(ULidarSensor::StaticClass()));
			// Lidar->NumLidarRays = 360; // Change the number of rays to 360
			// Register LIDAR sensor
			SensorManager.RegisterSensor(Lidar);
		}
		else if (ChildActor->GetName().StartsWith("BP_Camera"))
		{
			UCameraSensor *Camera = Cast<UCameraSensor>(ChildActor->GetComponentByClass(UCameraSensor::StaticClass()));
			// Register camera sensor
			SensorManager.RegisterSensor(Camera);
		}
	}
}
