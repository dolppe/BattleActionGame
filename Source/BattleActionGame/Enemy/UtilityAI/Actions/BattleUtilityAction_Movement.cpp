#include "BattleUtilityAction_Movement.h"

#include "AIController.h"
#include "BattleActionGame/Character/BattleCharacterBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleUtilityAction_Movement)

UBattleUtilityAction_MoveToTarget::UBattleUtilityAction_MoveToTarget()
{
}

void UBattleUtilityAction_MoveToTarget::StartAction()
{
	Super::StartAction();
	
	bStartMovement = false;
	
	SelectedTarget = CachedAIComponent->ConsiderList->SelectedTarget;
	MyCharacter = CachedAIComponent->ConsiderList->MyCharacter;

	if (SelectedTarget)
	{
		AAIController* AIController = Cast<AAIController>(MyCharacter->GetController());
		AIController->MoveToActor(SelectedTarget);
	}
	bStartMovement = true;
}

void UBattleUtilityAction_MoveToTarget::EndAction()
{

	
	Super::EndAction();
}

bool UBattleUtilityAction_MoveToTarget::TickAction(float DeltaTime)
{
	if (bStartMovement)
	{
		return true;
	}
	return false;
}
