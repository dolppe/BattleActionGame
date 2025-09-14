#include "AnimNotify_EndingState.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "BattleActionGame/BattleGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AnimNotify_EndingState)

void UAnimNotify_EndingState::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(MeshComp->GetOwner()))
	{
		ASC->RemoveLooseGameplayTag(FBattleGameplayTags::Get().Status_Action_Attack,3);
		ASC->RemoveLooseGameplayTag(FBattleGameplayTags::Get().Block_Movement);		
	}
}
