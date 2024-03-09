#pragma once

#include "CoreMinimal.h"

#include "Networking.h"
#include "Sockets.h"

class NEMO_SIMULATOR_API SocketManager
{
public:
	SocketManager(FString ServerIP, const int32 ServerPort);
	~SocketManager();

	bool StartServer();
	void StopServer();
	bool ListenForConnections();
	bool SendData(const uint8* Data, int32 DataSize);
	bool ReceiveData(uint8* Data, int32& DataSize);

	bool IsClientConnected();

	FIPv4Endpoint Endpoint;

private:
	FSocket* ServerSocket;
	FSocket* ConnectionSocket;

	bool CreateServerSocket();
	void CloseSocket(FSocket*& Socket);
};
