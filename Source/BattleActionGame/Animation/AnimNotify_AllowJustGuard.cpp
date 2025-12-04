#include "AnimNotify_AllowJustGuard.h"

#include "BattleActionGame/Combat/BattleCombatManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AnimNotify_AllowJustGuard)

void UAnimNotify_AllowJustGuard::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	if (AActor* Owner = MeshComp->GetOwner())
	{
		if (UBattleCombatManagerComponent* CombatManagerComponent = Cast<UBattleCombatManagerComponent>(Owner->GetComponentByClass(UBattleCombatManagerComponent::StaticClass())))
		{
			CombatManagerComponent->AllowGuardEvent();
		}
	}

	
}
