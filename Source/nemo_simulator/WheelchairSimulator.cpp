#include "WheelchairSimulator.h"

UWheelchairSimulator::UWheelchairSimulator()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UWheelchairSimulator::BeginPlay()
{
	Super::BeginPlay();
	NetworkStreamer.InitServer("127.0.0.1", 12345);

	if (!SetupLidarSensors()) {
		UE_LOG(LogTemp, Error, TEXT("Failed to setup Lidar sensors"));
	}

}

void UWheelchairSimulator::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UWheelchairSimulator::SetupLidarSensors()
{
	int NumLidarsSetup = 0;

	TArray<AActor*> ChildActors;
	GetOwner()->GetAttachedActors(ChildActors);

	for (AActor* ChildActor : ChildActors) {
		if (ChildActor->GetName().StartsWith("BP_LIDAR")) {
			ULidarSensor* Lidar = Cast<ULidarSensor>(ChildActor->GetComponentByClass(ULidarSensor::StaticClass()));
			FString LidarName = Lidar->LidarName;

			if (LidarName == "LIDAR_FR") {
				LidarFR = Lidar;
				NumLidarsSetup++;
			}
			else if (LidarName == "LIDAR_FL") {
				LidarFL = Lidar;
				NumLidarsSetup++;
			}
			else if (LidarName == "LIDAR_RR") {
				LidarRR = Lidar;
				NumLidarsSetup++;
			}
			else if (LidarName == "LIDAR_RL") {
				LidarRL = Lidar;
				NumLidarsSetup++;
			}
		}
	}

	return NumLidarsSetup == 4;
}
