#include "SocketManager.h"

#include "SocketSubsystem.h"

// Initialize SocketManager with given server IP address and port
SocketManager::SocketManager(FString ServerIP, const int32 ServerPort)
{
	FIPv4Address::Parse(ServerIP, Endpoint.Address);
	Endpoint.Port = ServerPort;
	ServerSocket = nullptr;
	ConnectionSocket = nullptr;

	UE_LOG(LogTemp, Log, TEXT("SocketManager initialized with server IP: %s, port: %d"), *ServerIP, ServerPort);
}

SocketManager::~SocketManager()
{
	StopServer();
	UE_LOG(LogTemp, Log, TEXT("SocketManager destroyed"));
}

bool SocketManager::StartServer()
{
	if (!CreateServerSocket()) {
		UE_LOG(LogTemp, Error, TEXT("Failed to create server socket"));
		return false;
	}

	UE_LOG(LogTemp, Log, TEXT("Server started"));
	return true;
}

void SocketManager::StopServer()
{
	UE_LOG(LogTemp, Log, TEXT("Stopping server"));

	// Close connection socket and server socket
	CloseSocket(ConnectionSocket);
	CloseSocket(ServerSocket);
}

bool SocketManager::SendData(const uint8* Data, const int32 DataSize)
{
	// Check if a client is connected
	if (!ConnectionSocket)
	{
		UE_LOG(LogTemp, Warning, TEXT("No client connected, cannot send data"));
		return false;
	}

	int32 Sent = 0;
	// Send data to connected client
	bool Success = ConnectionSocket->Send(Data, DataSize, Sent);
	return Success;
}

bool SocketManager::ReceiveData(uint8* Data, int32& DataSize)
{
	// Check if a client is connected
	if (!ConnectionSocket)
	{
		UE_LOG(LogTemp, Warning, TEXT("No client connected, cannot receive data"));
		return false;
	}

	int32 BytesRead = 0;
	// Receive data from the connected client
	bool Result = ConnectionSocket->Recv(Data, DataSize, BytesRead);
	DataSize = BytesRead;
	return Result;
}

bool SocketManager::CreateServerSocket()
{
	// Build the server socket
	ServerSocket = FTcpSocketBuilder(TEXT("UE5TCPCommServer"))
		.AsReusable()
		.BoundToEndpoint(Endpoint)
		.Listening(8);

	if (!ServerSocket)
	{
        UE_LOG(LogTemp, Error, TEXT("Failed to create server socket"));
		return false;
	}

	// Set the socket to non-blocking mode
	ServerSocket->SetNonBlocking(true);

	UE_LOG(LogTemp, Log, TEXT("Server socket created and bound to %s:%d"), *Endpoint.Address.ToString(), Endpoint.Port);

	return true;
}

void SocketManager::CloseSocket(FSocket*& Socket)
{
	// Check if socket is valid
	if (Socket)
	{
		// Close the connection
		Socket->Close();
		// Destroy the socket
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Socket);
		Socket = nullptr;
		UE_LOG(LogTemp, Log, TEXT("Socket closed and destroyed"));
	}
}

bool SocketManager::ListenForConnections()
{
	// Check if the server socket is valid
	if (!ServerSocket)
	{
		UE_LOG(LogTemp, Error, TEXT("Server socket is not valid, cannot listen for connections"));
		return false;
	}

	bool PendingConnection = false;
	// Check for pending connections
	if (!ServerSocket->HasPendingConnection(PendingConnection) || !PendingConnection)
	{
		return false;
	}

	UE_LOG(LogTemp, Log, TEXT("Pending connection found"));

	// Close the existing socket if it exists
	if (ConnectionSocket)
	{
		CloseSocket(ConnectionSocket);
		UE_LOG(LogTemp, Log, TEXT("Existing connection socket closed"));
	}

	// Accept the incoming connection
	ConnectionSocket = ServerSocket->Accept(TEXT("UE5TCPConnectionSocket"));

	bool Success = ConnectionSocket != nullptr;
	if (Success)
	{
		UE_LOG(LogTemp, Log, TEXT("Connection accepted"));
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Failed to accept connection"));
	}

	return Success;
}

bool SocketManager::IsClientConnected()
{
	// Check if the server socket and connection sockets are valid
	if (!ServerSocket) return false;
	if (!ConnectionSocket) return false;

	return true;
}