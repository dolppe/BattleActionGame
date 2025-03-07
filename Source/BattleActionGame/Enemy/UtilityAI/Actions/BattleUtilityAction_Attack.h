#pragma once

#include "BattleUtilityAction.h"
#include "BattleUtilityAction_Attack.generated.h"

enum class EAttackType : uint8;
class UBattleGameplayAbility_Attack_Parent;
class UAbilitySystemComponent;
struct FAttackAreaData;

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

	virtual bool TickAction(float DeltaTime) override;


	/*
	 * UtilityAction_Attack
	 */
	
	virtual void StartAgeTimer();

	virtual void StartAttack();

protected:

	UFUNCTION()
	virtual void UpdateAge();
	
	FTimerHandle TimerHandle;

	UPROPERTY(EditAnywhere)
	float AgeCycleTime = 10.f;

	UPROPERTY(EditAnywhere)
	float AgeRate = 0.5f;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UBattleGameplayAbility_Attack_Parent> GA_Attack;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> ASC;

	


private:
	
	
	
};

UCLASS()
class UBattleUtilityAction_AttackSingle : public UBattleUtilityAction_Attack
{
	GENERATED_BODY()

public:

	UBattleUtilityAction_AttackSingle();

	/*
	 * UtilityAction
	 */

	virtual void StartAction() override;

	virtual bool TickAction(float DeltaTime) override;

	/*
	 * UtilityAction_Attack
	 */
	 
	virtual void StartAttack() override;
	virtual void StartAgeTimer() override;

protected:

	virtual void UpdateAge() override;

	
private:
	

	
};


UCLASS()
class UBattleUtilityAction_AttackArea : public UBattleUtilityAction_Attack
{
	GENERATED_BODY()

public:

	UBattleUtilityAction_AttackArea();
	
	/*
	* UtilityAction
	*/

	virtual void StartAction() override;

	virtual bool TickAction(float DeltaTime) override;
	
	/*
	 * UtilityAction_Attack
	 */
	 
	virtual void StartAttack() override;
	virtual void StartAgeTimer() override;


protected:
	virtual void UpdateAge() override;

	UPROPERTY(EditAnywhere)
	float AreaRadius = 1.0f;

	UPROPERTY(EditAnywhere)
	int AreaNum = 1;


private:

	TArray<FAttackAreaData> GetBestSpots() const;
};
