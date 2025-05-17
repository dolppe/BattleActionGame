#pragma once

#include "BattleGameplayAbility_Attack_Parent.h"
#include "BattleGameplayAbility_ComboStrongAttack.generated.h"

class UBattleAbilityTask_HitCheck;
class UBattleCombatData;
struct FComboStrongAttack;

UCLASS()
class UBattleGameplayAbility_ComboStrongAttack : public UBattleGameplayAbility_Attack_Parent
{
	GENERATED_BODY()
public:

	UBattleGameplayAbility_ComboStrongAttack(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:

	virtual void OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& InData, FGameplayTag ApplicationTag) override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnTargetDataReady(const FGameplayAbilityTargetDataHandle& TargetData);
	

	virtual void OnCompleted() override;
	virtual void OnInterrupted() override;
	
	virtual void OnRep_AlreadyHitActors() override;

	virtual void StartHitCheck(FGameplayTag Channel, const FBattleVerbMessage& Notification) override;

	virtual void EndHitCheck(FGameplayTag Channel, const FBattleVerbMessage& Notification) override;


private:

	UBattleAbilityTask_HitCheck* HitCheckTask;

	const FComboStrongAttack* CurrentAttackData;


};

