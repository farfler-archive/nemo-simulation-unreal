#include "WheelchairSimulator.h"

UWheelchairSimulator::UWheelchairSimulator()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UWheelchairSimulator::BeginPlay()
{
	Super::BeginPlay();
}

void UWheelchairSimulator::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

