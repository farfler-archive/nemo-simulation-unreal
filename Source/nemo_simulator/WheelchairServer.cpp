#include "WheelchairServer.h"

#include "SocketSubsystem.h"

// Sets default values for this component's properties
UWheelchairServer::UWheelchairServer()
{
	// Enable ticking for this component
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts or the component is created
void UWheelchairServer::BeginPlay()
{
	Super::BeginPlay();

	// Get all child actors of the owner actor
	TArray<AActor*> ChildActors;
	GetOwner()->GetAttachedActors(ChildActors);

	int NumLidarsSet = 0; // Number of LIDAR components set

	for (AActor* ChildActor : ChildActors) {
		// Check if the child actor is a LIDAR component
		if (ChildActor->GetName().StartsWith("BP_LIDAR")) {
			// Get the LIDAR component from the child actor
			USensorLIDAR* LIDAR = Cast<USensorLIDAR>(ChildActor->GetComponentByClass(USensorLIDAR::StaticClass()));

			// Get the name of the LIDAR component
			FString LIDARSensorName = LIDAR->SensorName;

			// Set the LIDAR component based on its name
			if (LIDARSensorName == "LIDAR_FR")
			{
				LIDAR_FR = LIDAR; // Set the front right LIDAR component
				UE_LOG(LogTemp, Log, TEXT("LIDAR_FR: %s -> %s"), *LIDAR_FR->GetName(), *LIDAR_FR->SensorName);
				NumLidarsSet++;
			} else if (LIDARSensorName == "LIDAR_FL")
			{
				LIDAR_FL = LIDAR; // Set the front left LIDAR component
				UE_LOG(LogTemp, Log, TEXT("LIDAR_FL: %s -> %s"), *LIDAR_FL->GetName(), *LIDAR_FL->SensorName);
				NumLidarsSet++;
			} else if (LIDARSensorName == "LIDAR_RR")
			{
				LIDAR_RR = LIDAR; // Set the rear right LIDAR component
				UE_LOG(LogTemp, Log, TEXT("LIDAR_RR: %s -> %s"), *LIDAR_RR->GetName(), *LIDAR_RR->SensorName);
				NumLidarsSet++;
			} else if (LIDARSensorName == "LIDAR_RL")
			{
				LIDAR_RL = LIDAR; // Set the rear left LIDAR component
				UE_LOG(LogTemp, Log, TEXT("LIDAR_RL: %s -> %s"), *LIDAR_RL->GetName(), *LIDAR_RL->SensorName);
				NumLidarsSet++;
			}
		}
	}

	// Check if all LIDAR components are set
	if (NumLidarsSet == 4) {
		UE_LOG(LogTemp, Log, TEXT("All LIDAR sensors set"));

		SetupLIDARParams(); // Set up the LIDAR parameters

	} else {
		UE_LOG(LogTemp, Error, TEXT("Not all LIDAR sensors set"));
	}

	// Initialize and start the server
	FString ServerIP = TEXT("192.168.100.81");
	SocketMgr = new SocketManager(ServerIP, 12345);
	if (!SocketMgr->StartServer())
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to start the server"));
	}
	else
	{
		FIPv4Endpoint& ServerEndpoint = SocketMgr->Endpoint;
		FString EndpointString = *ServerEndpoint.ToString();
		UE_LOG(LogTemp, Log, TEXT("Started server at: %s"), *EndpointString)
	}
}

// Called when the game ends or the component is destroyed
void UWheelchairServer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (SocketMgr)
	{
		SocketMgr->StopServer();
	}

	Super::EndPlay(EndPlayReason);
}

// Called every frame
void UWheelchairServer::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Check if socket is valid
	if (SocketMgr) {
		SocketMgr->ListenForConnections(); // Listen for new connections
		
		// Check if a client is connected
		if (SocketMgr->IsClientConnected())
		{
			// SendLocation(); // Send the wheelchair's location to the client

			// Get the latest laser scan from the LIDAR components
			USensorLIDAR::SensorMsgLaserScan LidarScanRR = LIDAR_RR->GetLatestLaserScan();
			// LIDAR_RR->PrintLaserScan(LidarScanRR);
			// Send the latest laser scan to the client
			bool Success = SendLIDARScan(LidarScanRR);
			if (!Success) {
				UE_LOG(LogTemp, Warning, TEXT("Failed to send LIDAR scan"));
			}
		}
	}
}

// Send the wheelchair's location to the connected client
bool UWheelchairServer::SendLocation()
{
	if (!SocketMgr) return false; // Return false if no client is connected

	FVector ActorPosition = GetOwner()->GetActorLocation(); // Get the position of the owner actor

	struct Position {
		float x;
		float y;
		float z;
	};
	Position pos;
	pos.x = (float) ActorPosition.X;
	pos.y = (float) ActorPosition.Y;
	pos.z = (float) ActorPosition.Z;

	UE_LOG(LogTemp, Log, TEXT("Location packet: %f %f %f"), pos.x, pos.y, pos.z)

	// Send positon struct to client
	return SocketMgr->SendData((uint8*)&pos, sizeof(Position));
}

void SerializeLIDARScan(const USensorLIDAR::SensorMsgLaserScan msg, std::vector<char>& buffer)
{
	buffer.clear();
	size_t offset = 0;

	size_t header_frame_id_size = msg.header_frame_id.size();
	buffer.resize(offset + sizeof(header_frame_id_size));
	memcpy(buffer.data() + offset, &header_frame_id_size, sizeof(header_frame_id_size));
	offset += sizeof(header_frame_id_size);

	buffer.resize(offset + header_frame_id_size);
	memcpy(buffer.data() + offset, msg.header_frame_id.data(), header_frame_id_size);
	offset += header_frame_id_size;

	int32_t header_stamp_sec = msg.header_stamp_sec;
	buffer.resize(offset + sizeof(header_stamp_sec));
	memcpy(buffer.data() + offset, &msg.header_stamp_sec, sizeof(msg.header_stamp_sec));
	offset += sizeof(msg.header_stamp_sec);

	uint32_t header_stamp_nanosec = msg.header_stamp_nanosec;
	buffer.resize(offset + sizeof(header_stamp_nanosec));
	memcpy(buffer.data() + offset, &msg.header_stamp_nanosec, sizeof(msg.header_stamp_nanosec));
	offset += sizeof(msg.header_stamp_nanosec);

	float angle_min = msg.angle_min;
	buffer.resize(offset + sizeof(angle_min));
	memcpy(buffer.data() + offset, &msg.angle_min, sizeof(msg.angle_min));
	offset += sizeof(msg.angle_min);

	float angle_max = msg.angle_max;
	buffer.resize(offset + sizeof(angle_max));
	memcpy(buffer.data() + offset, &msg.angle_max, sizeof(msg.angle_max));
	offset += sizeof(msg.angle_max);

	float angle_increment = msg.angle_increment;
	buffer.resize(offset + sizeof(angle_increment));
	memcpy(buffer.data() + offset, &msg.angle_increment, sizeof(msg.angle_increment));
	offset += sizeof(msg.angle_increment);

	float time_increment = msg.time_increment;
	buffer.resize(offset + sizeof(time_increment));
	memcpy(buffer.data() + offset, &msg.time_increment, sizeof(msg.time_increment));
	offset += sizeof(msg.time_increment);

	float scan_time = msg.scan_time;
	buffer.resize(offset + sizeof(scan_time));
	memcpy(buffer.data() + offset, &msg.scan_time, sizeof(msg.scan_time));
	offset += sizeof(msg.scan_time);

	float range_min = msg.range_min;
	buffer.resize(offset + sizeof(range_min));
	memcpy(buffer.data() + offset, &msg.range_min, sizeof(msg.range_min));
	offset += sizeof(msg.range_min);

	float range_max = msg.range_max;
	buffer.resize(offset + sizeof(range_max));
	memcpy(buffer.data() + offset, &msg.range_max, sizeof(msg.range_max));
	offset += sizeof(msg.range_max);

	size_t ranges_size = msg.ranges.size();
	buffer.resize(offset + sizeof(ranges_size));
	memcpy(buffer.data() + offset, &ranges_size, sizeof(ranges_size));
	offset += sizeof(ranges_size);

	buffer.resize(offset + ranges_size * sizeof(float));
	memcpy(buffer.data() + offset, msg.ranges.data(), ranges_size * sizeof(float));
	offset += ranges_size * sizeof(float);

	size_t intensities_size = msg.intensities.size();
	buffer.resize(offset + sizeof(intensities_size));
	memcpy(buffer.data() + offset, &intensities_size, sizeof(intensities_size));
	offset += sizeof(intensities_size);

	buffer.resize(offset + intensities_size * sizeof(float));
	memcpy(buffer.data() + offset, msg.intensities.data(), intensities_size * sizeof(float));
	offset += intensities_size * sizeof(float);
}

bool UWheelchairServer::SendLIDARScan(USensorLIDAR::SensorMsgLaserScan LidarScan)
{
	if (!SocketMgr) return false; // Return false if no client is connected

	std::vector<char> buffer;
	SerializeLIDARScan(LidarScan, buffer);

	const size_t PacketSize = buffer.size() + sizeof(size_t);
	std::vector<char> packet(PacketSize);

	// Copy the packet size and LIDAR scan data into the packet
	std::memcpy(packet.data(), &PacketSize, sizeof(size_t));
	std::memcpy(packet.data() + sizeof(size_t), buffer.data(), buffer.size());

	UE_LOG(LogTemp, Log, TEXT("LIDAR packet size: %llu"), static_cast<unsigned long long>(PacketSize));

	// Send the packet to the client
	return SocketMgr->SendData(reinterpret_cast<const uint8*>(packet.data()), PacketSize);
}

void UWheelchairServer::SetupLIDARParams()
{
	// Set the number of rays of the LIDAR components
	LIDAR_FR->NumRays = 5000;
	LIDAR_FL->NumRays = 5000;
	LIDAR_RR->NumRays = 5000;
	LIDAR_RL->NumRays = 5000;

	bool bShouldDrawTrace = false; // Don't draw rays
	if (bShouldDrawTrace) {
		// Draw rays
		LIDAR_FR->bShouldDrawTrace = true;
		LIDAR_FL->bShouldDrawTrace = true;
		LIDAR_RR->bShouldDrawTrace = true;
		LIDAR_RL->bShouldDrawTrace = true;

		// Set colors of the LIDAR traces
		LIDAR_FR->RayColor = FColor::FromHex("ff595e"); // Red
		LIDAR_FL->RayColor = FColor::FromHex("ffca3a"); // Yellow
		LIDAR_RR->RayColor = FColor::FromHex("8ac926"); // Green
		LIDAR_RL->RayColor = FColor::FromHex("1982c4"); // Blue

		// Set the thickness of the LIDAR rays
		LIDAR_FR->RayThickness = 2.0f;
		LIDAR_FL->RayThickness = 2.0f;
		LIDAR_RR->RayThickness = 2.0f;
		LIDAR_RL->RayThickness = 2.0f;
	}
	else {
		// Don't draw rays
		LIDAR_FR->bShouldDrawTrace = false;
		LIDAR_FL->bShouldDrawTrace = false;
		LIDAR_RR->bShouldDrawTrace = false;
		LIDAR_RL->bShouldDrawTrace = false;
	}
}
