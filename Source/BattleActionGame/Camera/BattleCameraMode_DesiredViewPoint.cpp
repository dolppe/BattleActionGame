#include "BattleCameraMode_DesiredViewPoint.h"

#include "BattleActionGame/Character/BattleCharacter.h"
#include "BattleActionGame/Character/BattleHeroComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleCameraMode_DesiredViewPoint)

UBattleCameraMode_DesiredViewPoint::UBattleCameraMode_DesiredViewPoint(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UBattleCameraMode_DesiredViewPoint::UpdateView(float DeltaTime)
{
	FVector DesiredVector = DesiredLocation;
	FRotator DesiredRotator = DesiredRotation;

	if (!DesiredVector.IsZero())
	{
		View.Location = DesiredVector;
		View.FieldOfView = FieldOfView;
	}
	if (!DesiredRotator.IsZero())
	{
		View.Rotation = DesiredRotator;
		View.ControlRotation = DesiredRotator;
		View.FieldOfView = FieldOfView;
	}
	
	
}

void UBattleCameraMode_DesiredViewPoint::InitCameraMode(const FBattleCameraModeInitData& InitData)
{
	DesiredLocation = InitData.DesiredLocation;
	DesiredRotation = InitData.DesiredRotation;
	
}
