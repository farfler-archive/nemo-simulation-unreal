#include "SocketSubsystem.h"
#include "WheelchairServer.h"

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
