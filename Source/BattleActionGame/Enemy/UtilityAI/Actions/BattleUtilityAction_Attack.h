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
	
	virtual void StartAttack();

protected:
	
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

private:
	
	TArray<FVector> GetBestSpots() const;
	TArray<FVector> GetTargetSpots() const;

	UPROPERTY(EditAnywhere)
	float AreaRadius = 1.0f;
	
	UPROPERTY(EditAnywhere)
	int AreaNum = 1;

	
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

	virtual void StartAttack() override;


private:
	
	TArray<FVector> GetBestSpots() const;
	TArray<FVector> GetTargetSpots() const;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UBattleGameplayAbility_Special_Spawn> GA_Spawn;

	UPROPERTY(EditAnywhere)
	int SpawnActorNum = 1;
	
	UPROPERTY(EditAnywhere)
	int SpawnAreaRadius = 100.f;
	
};
