#pragma once

#include "BattleUtilityAction.h"
#include "BattleUtilityAction_Movement.generated.h"

UCLASS()
class UBattleUtilityAction_Move : public UBattleUtilityAction
{
	GENERATED_BODY()

public:

	UBattleUtilityAction_Move();

	/*
	 * UtilityAction
	 */

	virtual void StartAction() override;

	virtual void EndAction() override;

	virtual bool TickAction(float DeltaTime) override;

	
protected:
	

	
};



