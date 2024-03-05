// Fill out your copyright notice in the Description page of Project Settings.


#include "WheelchairServer.h"

// Sets default values for this component's properties
UWheelchairServer::UWheelchairServer()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void UWheelchairServer::BeginPlay()
{
	Super::BeginPlay();

	StartServer();
}


// Called every frame
void UWheelchairServer::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Check for incoming connections
	Listen();

	// Stream data to the client
	SendLocation();

	// Receive messages from the client and check for "STOP" message
	if (ConnectionSocket) {
		uint32 Size;
		while (ConnectionSocket->HasPendingData(Size)) {
			uint8* ReceivedData = new uint8[Size];
			int32 Read = 0;
			ConnectionSocket->Recv(ReceivedData, Size, Read);
			FString ReceivedMessage = FString(UTF8_TO_TCHAR(ReceivedData));
			UE_LOG(LogTemp, Warning, TEXT("Received: %s"), *ReceivedMessage);
			if (ReceivedMessage.StartsWith("STOP")) {
				StopServer();
			}
		}
	}
}

void UWheelchairServer::StartServer() {
	UE_LOG(LogTemp, Warning, TEXT("Starting server"));
	// Create a socket
	FIPv4Address::Parse(TEXT("127.0.0.1"), Endpoint.Address);
	Endpoint.Port = 12345;
	ServerSocket = FTcpSocketBuilder(TEXT("WheelchairLocationServer"))
		.AsReusable()
		.BoundToEndpoint(Endpoint)
		.Listening(8);

	// Check if the socket was created successfully
	if (!ServerSocket) {
		UE_LOG(LogTemp, Warning, TEXT("Failed to create server socket"));
		return;
	} else {
		UE_LOG(LogTemp, Warning, TEXT("Server socket created"));
	}

	// Set the socket to non-blocking
	ServerSocket->SetNonBlocking(true);
}

void UWheelchairServer::StopServer() {
	UE_LOG(LogTemp, Warning, TEXT("Stopping server"));
	if (ConnectionSocket) {
		ConnectionSocket->Close();
		// ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ConnectionSocket); // DO NOT USE AS IT CRASHES THE ENGINE
	}
	if (ServerSocket) {
		ServerSocket->Close();
		// ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ServerSocket);  // DO NOT USE AS IT CRASHES THE ENGINE
	}
}

bool UWheelchairServer::SendLocation() {
	if (!ConnectionSocket || !ConnectionSocket->GetConnectionState()) {
		return false;
	}

	// Encode the location as a string
	FVector ActorPositon = GetOwner()->GetActorLocation();
	FString LocationString = FString::Printf(TEXT("%f %f %f"), ActorPositon.X, ActorPositon.Y, ActorPositon.Z);
	TCHAR* SerializedChar = LocationString.GetCharArray().GetData();
	int32 Size = FCString::Strlen(SerializedChar) + 1;
	int32 Sent = 0;

	bool Result = ConnectionSocket->Send((uint8*)TCHAR_TO_UTF8(SerializedChar), Size, Sent);

	UE_LOG(LogTemp, Warning, TEXT("Sent: %s"), *LocationString);

	return Result;
}

void UWheelchairServer::Listen() {
	// Check for pending connections
	if (!ServerSocket) {
		return;
	}
	bool Pending;
	ServerSocket->HasPendingConnection(Pending);
	if (Pending) {
		UE_LOG(LogTemp, Warning, TEXT("Pending connection"));
		// Accept the connection
		ConnectionSocket = ServerSocket->Accept(TEXT("WheelchairLocationServer"));
		if (ConnectionSocket) {
			UE_LOG(LogTemp, Warning, TEXT("Connection established"));
		}
	}

	FPlatformProcess::Sleep(0.01);
}