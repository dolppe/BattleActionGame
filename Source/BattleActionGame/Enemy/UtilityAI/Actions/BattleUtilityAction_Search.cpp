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
	bSelected = false;
	for (AActor* BestTarget : BestTargets)
	{
		if (BestTarget->IsA(ABattleCharacter::StaticClass()))
		{
			CachedAIComponent->ConsiderList->SelectedTarget = Cast<ABattleCharacterBase>(BestTarget);
			break;
		}
	}
	bSelected = true;
}

void UBattleUtilityAction_SearchTarget::EndAction()
{
	Super::EndAction();
}

bool UBattleUtilityAction_SearchTarget::TickAction(float DeltaTime)
{
	if (bSelected)
	{
		return true;
	}
	return false;
}

float UBattleUtilityAction_SearchTarget::EvaluateScore(const UConsiderationFactors* ConsiderList)
{
	return Super::EvaluateScore(ConsiderList);
}
