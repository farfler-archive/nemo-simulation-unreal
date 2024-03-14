#include "NetworkStreamer.h"

NetworkStreamer::NetworkStreamer() : ServerSocket(nullptr), ConnectionSocket(nullptr)
{
}

NetworkStreamer::~NetworkStreamer()
{
	CloseServer();
}

bool NetworkStreamer::InitServer(const FString& ServerIP, const int32 Port)
{
	FIPv4Address::Parse(ServerIP, Endpoint.Address);
	Endpoint.Port = Port;

	ServerSocket = FTcpSocketBuilder(TEXT("NemoSimulatorServer"))
		.AsReusable()
		.BoundToEndpoint(Endpoint)
		.Listening(8);

	if (!ServerSocket)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create server socket"));
		return false;
	}

	ServerSocket->SetNonBlocking(true);

	UE_LOG(LogTemp, Log, TEXT("Server initialized"));

	return true;
}

bool NetworkStreamer::ListenForConnection()
{
	if (!ServerSocket)
	{
		return false;
	}

	bool PendingConnection = false;
	ServerSocket->HasPendingConnection(PendingConnection);

	if (!PendingConnection)
	{
		return false;
	}

	ConnectionSocket = ServerSocket->Accept(TEXT("NemoSimulatorConnection"));
	
	if (!ConnectionSocket)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to accept connection"));
		return false;
	}

	ConnectionSocket->SetNonBlocking(true);

	UE_LOG(LogTemp, Log, TEXT("Connection accepted"));

	return true;
}

bool NetworkStreamer::SendData(const std::vector<uint8_t>& Data)
{
	if (!ConnectionSocket)
	{
		return false;
	}

	int32 Sent = 0;
	return ConnectionSocket->Send(Data.data(), Data.size(), Sent);
}

void NetworkStreamer::CloseServer()
{
	CloseSocket(ServerSocket);
	CloseSocket(ConnectionSocket);
}

void NetworkStreamer::CloseSocket(FSocket*& Socket)
{
	if (Socket)
	{
		Socket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Socket);
		Socket = nullptr;
	}
}