#pragma once

// Include libraries for socket communication
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sockets.h"
#include "Networking.h"

#include "SensorLIDAR.h"

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WheelchairServer.generated.h"

/**
 * A server component designed for a Wheelchair simulation in Unreal Engine.
 * Handles TCP socket communication to send the wheelchair's location and receive commands.
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NEMO_SIMULATOR_API UWheelchairServer : public UActorComponent
{
	GENERATED_BODY()

public:	
	UWheelchairServer();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	// Socket for the server
	FSocket* ServerSocket;

	// Socket for the client connection
	FSocket* ConnectionSocket;
	
	// Endpoint (IP address and port) for the server
	FIPv4Endpoint Endpoint;

	// Flag to control whether the server should listen for incoming connections
	bool bShouldListen = true;

	void StartServer(); // Start the server and create a socket
	void StopServer(); // Stop the server and close the socket
	
	bool SendLocation(); // Send the wheelchair's location to the client

	void Listen(); // Listen for incoming client connections
	bool CheckConnection(); // Check if the connection is still alive

	// References to the LIDAR sensors
	USensorLIDAR* LIDAR_FR; // Front right
	USensorLIDAR* LIDAR_FL; // Front left
	USensorLIDAR* LIDAR_RR; // Rear right
	USensorLIDAR* LIDAR_RL; // Rear left

	void SetupLIDARParams(); // Set up the LIDAR parameters
};
