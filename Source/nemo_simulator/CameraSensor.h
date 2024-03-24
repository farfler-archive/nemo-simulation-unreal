#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/Texture2D.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "CameraImageData.h"
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

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Sensor")
	FString CameraName = "CAMERA"; // Name of the Camera sensor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Sensor")
	float FieldOfView = 90.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Sensor")
	float TargetCaptureFPS = 20.0f;

	CameraImageData::CameraImageData GetLatestImage();

private:
	UCameraComponent* CameraComponent;
	USceneCaptureComponent2D* SceneCaptureComponent;
	
	CameraImageData::CameraImageData LatestImage;
	
	float TimeSinceLastCapture = 0.0f;

	void CaptureImage();
};
