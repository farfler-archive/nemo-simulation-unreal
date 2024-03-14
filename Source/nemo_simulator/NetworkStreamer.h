#pragma once

#include "CoreMinimal.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Networking.h"

class NEMO_SIMULATOR_API NetworkStreamer
{
public:
	NetworkStreamer();
	~NetworkStreamer();

	bool InitServer(const FString& ServerIP, int32 ListenPort);
	bool ListenForConnection();
	bool SendData(const std::vector<uint8_t>& Data);
	void CloseServer();
	bool IsConnected() const;

private:
	FSocket* ServerSocket;
	FSocket* ConnectionSocket;

	FIPv4Endpoint Endpoint;

	void CloseSocket(FSocket*& Socket);
};
