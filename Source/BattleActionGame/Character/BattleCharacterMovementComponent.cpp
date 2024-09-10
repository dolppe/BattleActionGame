#include "BattleCharacterMovementComponent.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleCharacterMovementComponent)

UBattleCharacterMovementComponent::UBattleCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UBattleCharacterMovementComponent::PerformMovement(float DeltaTime)
{
	Super::PerformMovement(DeltaTime);
}
