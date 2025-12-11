#include "AnimNotify_AttackWarnSign.h"
#include "BattleActionGame/Combat/BattleCombatManagerComponent.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(AnimNotify_AttackWarnSign)

void UAnimNotify_AttackWarnSign::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	if (AActor* Owner = MeshComp->GetOwner())
	{
		if (UBattleCombatManagerComponent* CombatManagerComponent = Cast<UBattleCombatManagerComponent>(Owner->GetComponentByClass(UBattleCombatManagerComponent::StaticClass())))
		{
			CombatManagerComponent->OnAttackWarnSign();
		}
	}
	
	
}
