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

		{
			TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("AllowC"));
			AllowFuncC(MeshComp);
		}
		// {
		// 	TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("BlockC"));
		// 	BlockFuncC(MeshComp);	
		// }
		

		
		

		//ASC->RemoveActiveGameplayEffectBySourceEffect(RemoveGE1,ASC);
		//ASC->RemoveActiveGameplayEffectBySourceEffect(RemoveGE2,ASC);
		
	}
}

void UAnimNotify_EndingState::BlockFuncC(USkeletalMeshComponent* MeshComp)
{
	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(MeshComp->GetOwner());
	ASC->RemoveLooseGameplayTag(FBattleGameplayTags::Get().Status_Action_Attack,3);
	ASC->RemoveLooseGameplayTag(FBattleGameplayTags::Get().Block_Movement);
}

void UAnimNotify_EndingState::AllowFuncC(USkeletalMeshComponent* MeshComp)
{
	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(MeshComp->GetOwner());
	ASC->RemoveLooseGameplayTag(FBattleGameplayTags::Get().Status_Action_Attack,5);
	ASC->AddLooseGameplayTag(FBattleGameplayTags::Get().Allow_AllowRotation);
	ASC->AddLooseGameplayTag(FBattleGameplayTags::Get().Allow_Movement);
	
}
