#pragma once

#include "BattleGameplayAbility_Attack_Parent.h"
#include "BattleCombatData.h"
#include "BattleGameplayAbility_ComboAttack.generated.h"


class UBattleCombatManagerComponent;
class UBattleCombatData;
class UBattleAbilityTask_HitCheck;

UCLASS()
class UBattleGameplayAbility_ComboAttack : public UBattleGameplayAbility_Attack_Parent
{
	GENERATED_BODY()
public:

	UBattleGameplayAbility_ComboAttack(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	int GetComboIndex() const
	{
		return CurrentComboIndex;
	}
	
	void RequestNextCombo();
	
protected:
	
	FName GetNextSection();
	void StartComboTimer(FName MontageSectionName);
	void CheckComboInput();
	void AllowInput();

	UFUNCTION(Server, Reliable)
	void ServerRPCMontageSectionChanged(uint8 InCurrentComboIndex);

	virtual void OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& InData, FGameplayTag ApplicationTag) override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnTargetDataReady(const FGameplayAbilityTargetDataHandle& TargetData);

	UFUNCTION()
	void OnMontageCompleted(UAnimMontage* Montage, bool bInterrupted);
	UFUNCTION()
	void OnMontageBlendOut(UAnimMontage* Montage, bool bInterrupted);
	
	UFUNCTION()
	void OnRep_HasNextComboInput();

	
protected:
	
	uint8 CurrentComboIndex = 0;
	FTimerHandle ComboTimerHandle;
	FTimerHandle ComboAllowedTimerHandle;
	bool bAllowedInput;
	UPROPERTY(ReplicatedUsing=OnRep_HasNextComboInput)
	bool bHasNextComboInput;

	TObjectPtr<UBattleCombatManagerComponent> CurrentCombatManager;


private:

	const FComboAttack* CurrentComboAttackData;
	UBattleAbilityTask_HitCheck* HitCheckTask;
	
};
