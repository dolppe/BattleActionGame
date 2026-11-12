#include "BattleCameraMode_OffsetLookAt.h"

#include "BattleActionGame/BattleLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleCameraMode_OffsetLookAt)

UBattleCameraMode_OffsetLookAt::UBattleCameraMode_OffsetLookAt(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UBattleCameraMode_OffsetLookAt::UpdateView(float DeltaTime)
{
	AActor* TargetActor = GetTargetActor();
	
	FVector DesiredVector = CameraLocation;
	FRotator DesiredRotator = CameraRotation;
	
	BA_DEFAULT_LOG(LogBattle, Log, TEXT("%s => %s"), *DesiredVector.ToString(), *DesiredRotator.ToString());

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

void UBattleCameraMode_OffsetLookAt::InitCameraMode(const FBattleCameraModeInitData& InitData)
{
	AActor* MyActor = GetTargetActor();
	
	FRotator CharacterYaw(0.f, MyActor->GetActorRotation().Yaw,0.f);
	
	CameraLocation = MyActor->GetActorLocation() + CharacterYaw.RotateVector(MyLocationOffset);
	
	switch (LookAtType)
	{
	case ELookAtType::MyLocation:
		CameraRotation = (MyActor->GetActorLocation() - CameraLocation).Rotation();
		break;
	case ELookAtType::ImpactPointLocation:
		CameraRotation = (InitData.ImpactPointLocation - CameraLocation).Rotation();
		break;
	case ELookAtType::TargetLocation:
		CameraRotation = (InitData.StartTargetLocation - CameraLocation).Rotation();
		break;
	}	
	
	BA_DEFAULT_LOG(LogBattle, Log, TEXT("%s => %s, %s/%s"), *MyActor->GetActorNameOrLabel(), *this->GetName(), *CameraLocation.ToString(), *CameraRotation.ToString());
}
