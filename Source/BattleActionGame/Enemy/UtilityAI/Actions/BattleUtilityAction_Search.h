#pragma once

#include "BattleUtilityAction.h"
#include "BattleUtilityAction_Search.generated.h"

UCLASS()
class UBattleUtilityAction_SearchTarget : public UBattleUtilityAction
{
	GENERATED_BODY()

public:

	UBattleUtilityAction_SearchTarget();

	/*
	 * UtilityAction
	 */

	virtual void StartAction() override;

	virtual void EndAction() override;

	virtual bool TickAction(float DeltaTime) override;

	virtual float EvaluateScore(const UConsiderationFactors* ConsiderList) override;

	
protected:

	bool bSelected = false; 
	
};

