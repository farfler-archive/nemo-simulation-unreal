// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Include libraries for socket communication
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Networking.h"

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WheelchairServer.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NEMO_SIMULATOR_API UWheelchairServer : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWheelchairServer();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void StartServer();
	void StopServer();
	bool SendLocation(); // TODO: Implement for any data

private:
	FSocket* ServerSocket;
	FSocket* ConnectionSocket;
	FIPv4Endpoint Endpoint;
	bool bShouldListen = true;

	void Listen();
		
};
