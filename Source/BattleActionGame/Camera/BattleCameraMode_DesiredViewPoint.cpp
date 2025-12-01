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
	FVector PivotLocation = GetPivotLocation();
	FRotator PivotRotation = GetPivotRotation();

	AActor* TargetActor = GetTargetActor();

	if (CachedHeroComponent == nullptr)
	{
		CachedHeroComponent = Cast<UBattleHeroComponent>(TargetActor->GetComponentByClass(UBattleHeroComponent::StaticClass()));
	}

	if (CachedHeroComponent != nullptr)
	{
		FVector DesiredVector = CachedHeroComponent->GetDesiredLocation();
		FRotator DesiredRotator = CachedHeroComponent->GetDesiredRotation();

		if (!DesiredVector.IsZero())
		{
			View.Location = DesiredVector;
		}
		if (!DesiredRotator.IsZero())
		{
			View.Rotation = DesiredRotator;
			View.ControlRotation = DesiredRotator;
		}
	}	
	
}
