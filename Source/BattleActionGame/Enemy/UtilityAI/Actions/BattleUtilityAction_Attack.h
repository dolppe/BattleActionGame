#pragma once

#include "BattleUtilityAction.h"
#include "BattleUtilityAction_Attack.generated.h"

class UBattleGameplayAbility_Attack;

UCLASS()
class UBattleUtilityAction_Attack : public UBattleUtilityAction
{
	GENERATED_BODY()

public:

	UBattleUtilityAction_Attack();

	/*
	 * UtilityAction
	 */

	virtual void StartAction() override;

	virtual void EndAction() override;

	virtual bool TickAction(float DeltaTime) override;

	

protected:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UBattleGameplayAbility_Attack> GA_Attack;

	UPROPERTY(EditDefaultsOnly)
	float AttackRange;
	
	UPROPERTY(EditDefaultsOnly)
	float AttackRate;
	
};

