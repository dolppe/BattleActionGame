#pragma once

#include "BattleCameraMode.h"
#include "BattleCameraMode_OffsetLookAt.generated.h"

UENUM(BlueprintType)
enum ELookAtType
{
	MyLocation,
	TargetLocation,
	ImpactPointLocation,
};


UCLASS(Blueprintable)
class UBattleCameraMode_OffsetLookAt : public UBattleCameraMode
{
	GENERATED_BODY()
public:
	UBattleCameraMode_OffsetLookAt(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void UpdateView(float DeltaTime) override;
	
	virtual void InitCameraMode(const FBattleCameraModeInitData& InitData) override;
	
protected:
	
	UPROPERTY(EditAnywhere)
	TEnumAsByte<ELookAtType> LookAtType;
	
	UPROPERTY(EditAnywhere)
	FVector MyLocationOffset;
	
	FVector CameraLocation;
	FRotator CameraRotation;
	
	
};




