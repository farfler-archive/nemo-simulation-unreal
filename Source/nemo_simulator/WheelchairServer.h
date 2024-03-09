#pragma once

// Include libraries for socket communication
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SocketManager.h"

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
	// Socket manager
	SocketManager* SocketMgr;

	// Flag to control whether the server should listen for incoming connections
	bool bShouldListen = true;

	bool SendLocation(); // Send the wheelchair's location to the client

	// References to the LIDAR sensors
	USensorLIDAR* LIDAR_FR; // Front right
	USensorLIDAR* LIDAR_FL; // Front left
	USensorLIDAR* LIDAR_RR; // Rear right
	USensorLIDAR* LIDAR_RL; // Rear left

	void SetupLIDARParams(); // Set up the LIDAR parameters
};
