#include "AnimNotify_CommitState.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "BattleActionGame/BattleGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AnimNotify_CommitState)

void UAnimNotify_CommitState::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	BlockMovementWithRotationTag(MeshComp);
}

void UAnimNotify_CommitState::BlockMovementWithRotationTag(USkeletalMeshComponent* MeshComp)
{
	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(MeshComp->GetOwner()))
	{
		ASC->SetLooseGameplayTagCount(FBattleGameplayTags::Get().Block_Movement_AllowRotation, 0);
		ASC->AddLooseGameplayTag(FBattleGameplayTags::Get().Block_Movement);	
	}
	
}
