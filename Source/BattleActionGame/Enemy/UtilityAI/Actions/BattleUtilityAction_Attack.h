#pragma once

#include "BattleUtilityAction.h"
#include "BattleUtilityAction_Attack.generated.h"

enum class EAttackType : uint8;
class UBattleGameplayAbility_Attack_Parent;
class UAbilitySystemComponent;
struct FAttackAreaData;

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

	void UpdateAge();

protected:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UBattleGameplayAbility_Attack_Parent> GA_Attack;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> ASC;
	
	UPROPERTY(EditDefaultsOnly)
	float CoolTime = 7.0f;
	
};


UCLASS()
class UBattleUtilityAction_AttackArea : public UBattleUtilityAction_AttackSingle
{
	GENERATED_BODY()

public:

	UBattleUtilityAction_AttackArea();

	TArray<FAttackAreaData> GetBestSpots();

	virtual void StartAction() override;

	virtual void EndAction() override;

	virtual bool TickAction(float DeltaTime) override;

	virtual float EvaluateScore(const UConsiderationFactors* ConsiderList) override;

	void UpdateAge();

protected:

	UPROPERTY(EditAnywhere)
	float AreaRadius = 1.0f;

	UPROPERTY(EditAnywhere)
	int AreaNum = 1;
};
