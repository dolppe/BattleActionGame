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
	AActor* SelectTarget = nullptr;
	for (AActor* BestTarget : BestTargets)
	{
		if (BestTarget == nullptr)
		{
			continue;
		}
		if (BestTarget->IsA(ABattleCharacter::StaticClass()))
		{
			SelectTarget = BestTarget;
			break;
		}
	}

	if (SelectTarget == nullptr)
	{
		// 도주시 거리가 멀어져서 Distance 안쪽으로 Target이 없는 경우 캐싱해둔 Target 제거
		CachedAIComponent->ConsiderList->SelectedTarget = nullptr;
	}
	else
	{
		CachedAIComponent->ConsiderList->SelectedTarget = Cast<ABattleCharacterBase>(SelectTarget);
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
