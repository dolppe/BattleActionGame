#include "BattleUtilityAction_Search.h"

#include "BattleActionGame/BattleLogChannels.h"
#include "BattleActionGame/Character/BattleCharacter.h"
#include "BattleActionGame/Enemy/UtilityAI/BattleUtilityAxis.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleUtilityAction_Search)

UBattleUtilityAction_SearchTarget::UBattleUtilityAction_SearchTarget()
{
}

void UBattleUtilityAction_SearchTarget::StartAction()
{
	Super::StartAction();

	AActor* SelectTarget = nullptr;
	for (AActor* BestTarget : BestTargets)
	{
		if (BestTarget == nullptr)
		{
			continue;
		}
		if (BestTarget->IsA(ABattleCharacterBase::StaticClass()))
		{
			SelectTarget = BestTarget;
			break;
		}
	}

	if (SelectTarget == nullptr)
	{
		for (AActor* Target : CachedAIComponent->ConsiderList->TargetActors)
		{
			SelectTarget = Target;
			break;
		}
		
		if (SelectTarget != nullptr)
		{
			CachedAIComponent->ConsiderList->SelectedTarget = Cast<ABattleCharacterBase>(SelectTarget);	
		}
	}
	else
	{
		CachedAIComponent->ConsiderList->SelectedTarget = Cast<ABattleCharacterBase>(SelectTarget);
	}

	bIsCompletedAction = true;
}

void UBattleUtilityAction_SearchTarget::EndAction()
{
	Super::EndAction();
}

void UBattleUtilityAction_SearchTarget::TickAction(float DeltaTime)
{

}

float UBattleUtilityAction_SearchTarget::EvaluateScore(const UConsiderationFactors* ConsiderList)
{
	return Super::EvaluateScore(ConsiderList);
}
