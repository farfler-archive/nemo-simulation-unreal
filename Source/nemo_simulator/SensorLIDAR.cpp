// Fill out your copyright notice in the Description page of Project Settings.


#include "SensorLIDAR.h"

// Sets default values for this component's properties
USensorLIDAR::USensorLIDAR()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USensorLIDAR::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void USensorLIDAR::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (int i = 0; i < 360; i++) {
		float Distance = TraceLine(i);
	}
}

float USensorLIDAR::TraceLine(float Angle)
{
	FVector Start = GetOwner()->GetActorLocation();

	FRotator OwnerRotation = GetOwner()->GetActorRotation();
	FVector EndOffset = FVector(MaxDistance * FMath::Cos(FMath::DegreesToRadians(Angle)), MaxDistance * FMath::Sin(FMath::DegreesToRadians(Angle)), 0.0f);
	FVector End = Start + OwnerRotation.RotateVector(EndOffset);

	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(GetOwner());

	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility, CollisionParams);

	if (HitResult.bBlockingHit) {
		if (bShouldDrawTrace) {
			DrawDebugLine(GetWorld(), Start, HitResult.ImpactPoint, FColor::Green, false, 0.1f, 0, 0.2f);
			DrawDebugPoint(GetWorld(), HitResult.ImpactPoint, 10, FColor::Green, false, 0.1f, 0);
		}
		return HitResult.Distance;
	}
	else {
		if (bShouldDrawTrace) {
			DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 0.1f, 0, 1);
		}
		return MaxDistance;
	}
}