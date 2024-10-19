#pragma once

#include "BattleUtilityAction.h"
#include "BattleUtilityAction_Movement.generated.h"

UCLASS()
class UBattleUtilityAction_MoveToTarget : public UBattleUtilityAction
{
	GENERATED_BODY()

public:

	UBattleUtilityAction_MoveToTarget();

	/*
	 * UtilityAction
	 */

	virtual void StartAction() override;

	virtual void EndAction() override;

	virtual bool TickAction(float DeltaTime) override;

	
protected:


	TObjectPtr<ABattleCharacterBase> SelectedTarget;

	TObjectPtr<ABattleCharacterBase> MyCharacter;

	bool bStartMovement;

	
};



