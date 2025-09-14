#include "AnimNotify_OpeningState.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "BattleActionGame/BattleGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AnimNotify_OpeningState)

void UAnimNotify_OpeningState::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(MeshComp->GetOwner()))
	{
		ASC->AddLooseGameplayTag(FBattleGameplayTags::Get().Block_Movement_AllowRotation);
		ASC->AddLooseGameplayTag(FBattleGameplayTags::Get().Status_Action_Attack);
	}
	
	
}
