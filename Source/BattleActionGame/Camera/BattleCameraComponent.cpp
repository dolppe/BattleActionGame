#include "BattleCameraComponent.h"
#include "BattleCameraMode.h"
#include "BattleActionGame/BattleLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleCameraComponent)

UBattleCameraComponent::UBattleCameraComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer), CameraModeStack(nullptr)
{
}

void UBattleCameraComponent::UpdateCameraModes()
{
	check(CameraModeStack);
	
	if (DetermineCameraModeDelegate.IsBound())
	{
		// CameraMode의 Class를 가져오는 것임.
		// - HeroComponent의 멤버함수에 바인딩 되어 있음.
		if (TSubclassOf<UBattleCameraMode> CameraMode = DetermineCameraModeDelegate.Execute())
		{
			CameraModeStack->PushCameraMode(CameraMode);
		}
	}
	
}

void UBattleCameraComponent::GetBlendInfo(float& OutWeightOfTopLayer, FGameplayTag& OutTagOfTopLayer) const
{
	check(CameraModeStack);
	CameraModeStack->GetBlendInfo(OutWeightOfTopLayer, OutTagOfTopLayer);
}

void UBattleCameraComponent::OnRegister()
{
	Super::OnRegister();
	
	if (!CameraModeStack)
	{
		// Beginplay와 같은 초기화가 필요 없으므로 그냥 NewObject로 생성.
		CameraModeStack = NewObject<UBattleCameraModeStack>(this);
	}
}

PRAGMA_DISABLE_OPTIMIZATION

void UBattleCameraComponent::GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView)
{
	check(CameraModeStack);
	UpdateCameraModes();

	FBattleCameraModeView CameraModeView;
	CameraModeStack->EvaluateStack(DeltaTime, CameraModeView);

	if (APawn* TargetPawn = Cast<APawn>(GetTargetActor()))
	{
		if (APlayerController* PC = TargetPawn->GetController<APlayerController>())
		{
			// PlayerController의 COntrolRotation을 계산된 CameraModeView의 ControlRotation으로 업데이트
			// PC가 Possess하고 있는 Pawn의 RootComponent에 ControRotation을 반영한다.
			// (조종하고 있는 캐릭터에 회전을 시킴)
			PC->SetControlRotation(CameraModeView.ControlRotation);
		}
	}

	// 카메라에 Location과 Rotation 반영
	// CameraComponent에 대해 Parent의 SceneGraph 관계를 고려하며 업데이트를 진행함.
	SetWorldLocationAndRotation(CameraModeView.Location, CameraModeView.Rotation);

	FieldOfView = CameraModeView.FieldOfView;

	// ControlRotation vs Rotation의 차이점
	// ControlRotation => PC가 조종하는 Pawn의 Rotation에 적용할 값이다.
	// Rotation은 Camera에 반영하는 Rotation이다.


	// FMinimalViewInfo를 업데이트 해줌.
	// - CameraComponent의 변화 사항을 반영해서 최종 렌더링까지 반영함.
	DesiredView.Location = CameraModeView.Location;
	DesiredView.Rotation = CameraModeView.Rotation;
	DesiredView.FOV = CameraModeView.FieldOfView;
	DesiredView.OrthoWidth = OrthoWidth;
	DesiredView.OrthoNearClipPlane = OrthoNearClipPlane;
	DesiredView.OrthoFarClipPlane = OrthoFarClipPlane;
	DesiredView.AspectRatio = AspectRatio;
	DesiredView.bConstrainAspectRatio = bConstrainAspectRatio;
	DesiredView.bUseFieldOfViewForLOD = bUseFieldOfViewForLOD;
	DesiredView.ProjectionMode = ProjectionMode;
	DesiredView.PostProcessBlendWeight = PostProcessBlendWeight;
	if (PostProcessBlendWeight > 0.0f)
	{
		DesiredView.PostProcessSettings = PostProcessSettings;
	}
}

PRAGMA_ENABLE_OPTIMIZATION
