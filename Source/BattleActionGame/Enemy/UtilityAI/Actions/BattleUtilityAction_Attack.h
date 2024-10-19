#pragma once

#include "BattleUtilityAction.h"
#include "BattleUtilityAction_Attack.generated.h"

class UBattleGameplayAbility_Attack_Parent;
class UAbilitySystemComponent;

UCLASS()
class UBattleUtilityAction_AttackSingle : public UBattleUtilityAction
{
	GENERATED_BODY()

public:

	UBattleUtilityAction_AttackSingle();

	/*
	 * UtilityAction
	 */

	virtual void StartAction() override;

	virtual void EndAction() override;

	virtual bool TickAction(float DeltaTime) override;

	virtual float EvaluateScore(const UConsiderationFactors* ConsiderList) override;

	

protected:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UBattleGameplayAbility_Attack_Parent> GA_Attack;

	UPROPERTY(EditDefaultsOnly)
	float AttackRange;
	
	UPROPERTY(EditDefaultsOnly)
	float AttackRate;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> ASC;
	
};

