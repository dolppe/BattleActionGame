#pragma once

#include "BattleUtilityAction.h"
#include "BattleUtilityAction_Attack.generated.h"

class UBattleGameplayAbility_Special_Spawn;
enum class EAttackType : uint8;
class UBattleGameplayAbility_Attack_Parent;
class UAbilitySystemComponent;

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

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> ASC;

	
	
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

	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UBattleGameplayAbility_Attack_Parent> GA_Attack;

	
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

private:

	void GetAreaData();
	TArray<FVector> GetBestSpots() const;
	TArray<FVector> GetTargetSpots() const;

	float AreaRadius = 1.0f;
	int AreaNum = 1;
	bool IsSetAreaData = false;

	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UBattleGameplayAbility_Attack_Parent> GA_Attack;
	
};

UCLASS()
class UBattleUtilityAction_SpawnActor : public UBattleUtilityAction_Attack
{
	GENERATED_BODY()
public:

	UBattleUtilityAction_SpawnActor();

	/*
	 * UtilityAction
	 */

	virtual void StartAction() override;

	virtual bool TickAction(float DeltaTime) override;


	/*
	 * UtilityAction_Attack
	 */
	
	virtual void StartAgeTimer() override;

	virtual void StartAttack() override;

protected:
	
	virtual void UpdateAge() override;

private:

	void GetSpawnData();
	TArray<FVector> GetBestSpots() const;
	TArray<FVector> GetTargetSpots() const;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UBattleGameplayAbility_Special_Spawn> GA_Spawn;

	int SpawnActorNum = 1;
	int SpawnAreaRadius = 100.f;
	bool IsSetSpawnData = false;
	
	
};
