#include "CameraSensor.h"

UCameraSensor::UCameraSensor()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCameraSensor::BeginPlay()
{
	Super::BeginPlay();
}

void UCameraSensor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

