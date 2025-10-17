#include "AnimNotify_CommitState.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "BattleActionGame/BattleGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AnimNotify_CommitState)

void UAnimNotify_CommitState::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(MeshComp->GetOwner()))
	{
		{
			TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("AllowB"));
			AllowFuncB(MeshComp);
			
		}
		
		// {
		// 	TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("BlockB"));
		// 	BlockFuncB(MeshComp);
		// }
		
		
		

		

		//ASC->ApplyGameplayEffectToSelf(ApplyGE.GetDefaultObject(), 1.0f, ASC->MakeEffectContext());
		//ASC->RemoveActiveGameplayEffectBySourceEffect(RemoveGE,ASC);
	}
}

void UAnimNotify_CommitState::BlockFuncB(USkeletalMeshComponent* MeshComp)
{
	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(MeshComp->GetOwner());
	ASC->RemoveLooseGameplayTag(FBattleGameplayTags::Get().Block_Movement_AllowRotation);
	ASC->AddLooseGameplayTag(FBattleGameplayTags::Get().Block_Movement);
}

void UAnimNotify_CommitState::AllowFuncB(USkeletalMeshComponent* MeshComp)
{
	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(MeshComp->GetOwner());
	ASC->RemoveLooseGameplayTag(FBattleGameplayTags::Get().Allow_AllowRotation,5);
}

