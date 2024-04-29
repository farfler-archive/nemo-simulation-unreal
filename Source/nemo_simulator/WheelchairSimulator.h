#pragma once

#include "CoreMinimal.h"
#include "NetworkStreamer.h"
#include "SensorManager.h"

#include "Components/ActorComponent.h"
#include "WheelchairSimulator.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NEMO_SIMULATOR_API UWheelchairSimulator : public UActorComponent
{
	GENERATED_BODY()

public:	
	UWheelchairSimulator();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	float LatestUpdateTime = 0;

	// Network socket for streaming data
	NetworkStreamer NetworkStreamerFR;
	NetworkStreamer NetworkStreamerFL;
	NetworkStreamer NetworkStreamerRR;
	NetworkStreamer NetworkStreamerRL;
	NetworkStreamer NetworkStreamerCamera;

	// Network streamer for receiving velocity commands
	NetworkStreamer NetworkStreamerVelocity;

	// Sensor manager
	SensorManager SensorManager;

	void SetupSensors();
};
