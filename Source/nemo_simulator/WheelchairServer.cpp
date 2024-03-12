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


	// Limit to max 30 updates per second
	if (FPlatformTime::Seconds() - LastSendTime < 1.0f / 30.0f) return;

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

	LastSendTime = FPlatformTime::Seconds();
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

bool IsLittleEndian() {
	uint16_t test = 0x1; // Use a 16-bit integer with a value that will reveal byte ordering
	return *((uint8_t*)&test) == 0x1; // Check the first byte; if it's 1, it's little-endian
}

// Implementation of htonl: Host TO Network Long of uint32_t
uint32_t my_htonl(uint32_t hostlong) {
	if (IsLittleEndian()) {
		// Convert from little-endian to big-endian
		return ((hostlong & 0x000000FF) << 24) | ((hostlong & 0x0000FF00) << 8) | ((hostlong & 0x00FF0000) >> 8) | ((hostlong & 0xFF000000) >> 24);
	}
	else {
		// Already big-endian
		return hostlong;
	}
}	

bool UWheelchairServer::SendLIDARScan(USensorLIDAR::SensorMsgLaserScan LidarScan)
{
	if (!SocketMgr) return false; // Return false if no client is connected

	std::vector<char> buffer = USensorLIDAR::SerializeLaserScan(LidarScan);

	uint32_t packet_size = buffer.size();
	uint32_t packet_size_net = my_htonl(static_cast<uint32_t>(packet_size)); // Convert packet size to network byte order

	// Insert the size at the beginning of the buffer
	buffer.insert(buffer.begin(), reinterpret_cast<char*>(&packet_size_net), reinterpret_cast<char*>(&packet_size_net) + sizeof(packet_size_net));

	UE_LOG(LogTemp, Log, TEXT("LIDAR packet size: %llu"), static_cast<unsigned long long>(packet_size + sizeof(uint32_t)));

	// Send the packet to the client
	bool sendResult = SocketMgr->SendData(reinterpret_cast<const uint8*>(buffer.data()), buffer.size());

	return sendResult;
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
