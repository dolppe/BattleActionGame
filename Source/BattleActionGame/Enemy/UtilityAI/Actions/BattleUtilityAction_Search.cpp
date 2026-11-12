#include "BattleUtilityAction_Search.h"

#include "BattleActionGame/BattleLogChannels.h"
#include "BattleActionGame/Character/BattleCharacter.h"
#include "BattleActionGame/Enemy/UtilityAI/BattleConsiderationFactors.h"
#include "BattleActionGame/Enemy/UtilityAI/BattleUtilityAxis.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleUtilityAction_Search)

UBattleUtilityAction_SearchTarget::UBattleUtilityAction_SearchTarget()
{
}

void UBattleUtilityAction_SearchTarget::StartAction()
{
	Super::StartAction();
	
	UConsiderationFactors* ConsiderList = ConsiderationFactors.Get();
	
	if (!IsValid(ConsiderList))
	{
		bIsCompletedAction = true;
		return;
	}
	
	ABattleCharacterBase* SelectedTarget = ConsiderList->GetSelectedTarget();
	
	if (!IsValid(SelectedTarget))
	{
		bIsCompletedAction = true;
		return;
	}

	AActor* BestTarget = BestTargets[EAxisType::Target];
	
	if (BestTarget == nullptr)
	{
		for (int32 Idx = 0; Idx < ConsiderList->GetCandidateArrNum(EAxisType::Target); ++Idx)
		{
			AActor* TargetActor = ConsiderList->GetCandidatePtr(EAxisType::Target, Idx);
			if (!IsValid(TargetActor))
			{
				continue;
			}
			BestTarget = TargetActor;
			break;
		}
	}
	
	if (BestTarget == nullptr)
	{
		bIsCompletedAction = true;
		return;
	}
	
	if (ABattleCharacterBase* BestCharacter = Cast<ABattleCharacterBase>(BestTarget))
	{
		ConsiderList->SetSelectedTarget(BestCharacter);	
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
