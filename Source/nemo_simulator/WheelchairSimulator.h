#pragma once

#include "CoreMinimal.h"
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

		
};
