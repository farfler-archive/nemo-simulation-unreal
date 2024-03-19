#include "CameraSensor.h"

UCameraSensor::UCameraSensor()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCameraSensor::BeginPlay()
{
	Super::BeginPlay();

	CameraComponent = GetOwner()->FindComponentByClass<UCameraComponent>();
	if (!CameraComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("No camera component found on %s"), *GetOwner()->GetName());
		return;
	}

	// Initialize the scene capture component
	SceneCaptureComponent = NewObject<USceneCaptureComponent2D>(this);
	SceneCaptureComponent->SetupAttachment(CameraComponent);
	SceneCaptureComponent->SetRelativeLocation(FVector::ZeroVector);
	SceneCaptureComponent->SetRelativeRotation(FRotator::ZeroRotator);
	SceneCaptureComponent->FOVAngle = FieldOfView;
	SceneCaptureComponent->bCaptureEveryFrame = false;
	SceneCaptureComponent->bCaptureOnMovement = false;
	// Fix for issue "ScreenCapture has no viewstate..."
	SceneCaptureComponent->bAlwaysPersistRenderingState = true;
	// Fix for global illumination not being captured
	SceneCaptureComponent->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;

	UTextureRenderTarget2D* RenderTarget = NewObject<UTextureRenderTarget2D>();
	RenderTarget->InitAutoFormat(640, 480);
	SceneCaptureComponent->TextureTarget = RenderTarget;

	SceneCaptureComponent->RegisterComponent(); // Make sure the component is registered and will tick
}

void UCameraSensor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TimeSinceLastCapture += DeltaTime;
	if (TimeSinceLastCapture >= (1.0f / TargetCaptureFPS))
	{
		CaptureImage();
		TimeSinceLastCapture = 0.0f;
	}
}

void UCameraSensor::CaptureImage()
{
	if (!SceneCaptureComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("No scene capture component found on %s"), *GetOwner()->GetName());
		return;
	}

	// Capture the image
	SceneCaptureComponent->CaptureScene();
	UTextureRenderTarget2D* RenderTarget = SceneCaptureComponent->TextureTarget;
	RenderTarget->TargetGamma = 2.2f;
	FTextureRenderTargetResource* RenderTargetResource = RenderTarget->GameThread_GetRenderTargetResource();
	FReadSurfaceDataFlags ReadFlags;
	ReadFlags.SetLinearToGamma(true);

	TArray<FColor> SurfaceData;
	if (RenderTargetResource->ReadPixels(SurfaceData, ReadFlags))
	{
		LatestImage = SurfaceData;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to read pixels from render target"));
	}
}
