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

		// Set colors of the LIDAR components
		LIDAR_FR->RayColor = FColor::FromHex("ff595e");
		LIDAR_FL->RayColor = FColor::FromHex("ffca3a");
		LIDAR_RR->RayColor = FColor::FromHex("8ac926");
		LIDAR_RL->RayColor = FColor::FromHex("1982c4");

		// Set the thickness of the LIDAR rays
		LIDAR_FR->RayThickness = 2.0f;
		LIDAR_FL->RayThickness = 2.0f;
		LIDAR_RR->RayThickness = 2.0f;
		LIDAR_RL->RayThickness = 2.0f;

		// Set the number of rays of the LIDAR components
		LIDAR_FR->NumRays = 36;
		LIDAR_FL->NumRays = 36;
		LIDAR_RR->NumRays = 36;
		LIDAR_RL->NumRays = 36;

	} else {
		UE_LOG(LogTemp, Error, TEXT("Not all LIDAR sensors set"));
	}

	StartServer();
}


// Called when the game ends or the component is destroyed
void UWheelchairServer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopServer();
	Super::EndPlay(EndPlayReason);
}

// Called every frame
void UWheelchairServer::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	Listen(); // Listen for incoming client connections

	// If a client is connected
	if (ConnectionSocket) {
		// Check if the connection is still alive
		if (CheckConnection()) {
			SendLocation(); // Send the wheelchair's location to the client
		}
	}
}

// Start the server and create a socket
void UWheelchairServer::StartServer()
{
	FIPv4Address::Parse(TEXT("127.0.0.1"), Endpoint.Address); // Set the server's IP address
	Endpoint.Port = 12345; // Set the server's port

	// Create a reusable TCP socket bound to the specified endpoint and listen for incoming connections
	ServerSocket = FTcpSocketBuilder(TEXT("WheelchairLocationServerSocket"))
		.AsReusable()
		.BoundToEndpoint(Endpoint)
		.Listening(8);

	ServerSocket->SetNonBlocking(true); // Set the server socket to non-blocking mode

	UE_LOG(LogTemp, Log, TEXT("Server started at %s:%d"), *Endpoint.Address.ToString(), Endpoint.Port);
}

// Stop the server and clean up the sockets
void UWheelchairServer::StopServer()
{
	// If a client is connected, close and destroy the connection socket
	if (ConnectionSocket) {
		ConnectionSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ConnectionSocket);
		ConnectionSocket = nullptr;
	}

	// Close and destroy the server socket
	if (ServerSocket) {
		ServerSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ServerSocket);
		ServerSocket = nullptr;
	}

	UE_LOG(LogTemp, Log, TEXT("Server stopped"));
}

// Send the wheelchair's location to the connected client
bool UWheelchairServer::SendLocation()
{
	if (!ConnectionSocket) return false; // Return false if no client is connected

	FVector ActorPosition = GetOwner()->GetActorLocation(); // Get the position of the owner actor
	FString LocationString = FString::Printf(TEXT("%f %f %f"), ActorPosition.X, ActorPosition.Y, ActorPosition.Z); // Convert the position to a string

	int32 Size = FCString::Strlen(*LocationString) + 1; // Size of the location string
	int32 Sent = 0;

	// Send the location string to the client
	return ConnectionSocket->Send((uint8*)TCHAR_TO_UTF8(*LocationString), Size, Sent);
}

// Listen for incoming client connections
void UWheelchairServer::Listen()
{
	if (!ServerSocket || !bShouldListen) return; // Return if the server socket is not valid or if the server should not listen

	bool Pending;
	ServerSocket->HasPendingConnection(Pending);

	if (Pending) {
		UE_LOG(LogTemp, Log, TEXT("Pending connection"))

		// If there is a pending connection, accept it and create a connection socket
		ConnectionSocket = ServerSocket->Accept(TEXT("WheelchairLocationConnectionSocket"));
		
		if (ConnectionSocket) {
			UE_LOG(LogTemp, Log, TEXT("Client connected from %s"), *ConnectionSocket->GetDescription())
		}
	}
}

// Check if the client connection is still alive
bool UWheelchairServer::CheckConnection()
{
	if (!ConnectionSocket) return false; // Return false if no client is connected

	const char* HearbeatData = "HB";
	int32 Size = 2;
	int32 Sent = 0;

	// Send the heartbeat data to the client
	return ConnectionSocket->Send((uint8*)HearbeatData, Size, Sent);
}
