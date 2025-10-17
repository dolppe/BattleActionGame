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
		{
			TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("AllowA"));
			AllowFuncA(MeshComp);
		}

		// {
		// 	TRACE_CPUPROFILER_EVENT_SCOPE(TEXT("BlockA"));
		// 	BlockFuncA(MeshComp);
		// }
		
		
		
		

		//ASC->ApplyGameplayEffectToSelf(ApplyGE1.GetDefaultObject(), 1.0f, ASC->MakeEffectContext());
		//ASC->ApplyGameplayEffectToSelf(ApplyGE2.GetDefaultObject(), 1.0f, ASC->MakeEffectContext());
	}
	
	
}

void UAnimNotify_OpeningState::BlockFuncA(USkeletalMeshComponent* MeshComp)
{
	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(MeshComp->GetOwner());
	ASC->AddLooseGameplayTag(FBattleGameplayTags::Get().Block_Movement_AllowRotation);
	ASC->AddLooseGameplayTag(FBattleGameplayTags::Get().Status_Action_Attack);
}

void UAnimNotify_OpeningState::AllowFuncA(USkeletalMeshComponent* MeshComp)
{
	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(MeshComp->GetOwner());
	ASC->RemoveLooseGameplayTag(FBattleGameplayTags::Get().Allow_Movement, 5);
	ASC->AddLooseGameplayTag(FBattleGameplayTags::Get().Status_Action_Attack);
}
