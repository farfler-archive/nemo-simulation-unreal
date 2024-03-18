#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CameraSensor.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NEMO_SIMULATOR_API UCameraSensor : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCameraSensor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
