#pragma once

#include "BattleComboData.h"
#include "BattleActionGame/AbilitySystem/Abilities/BattleGameplayAbility.h"
#include "BattleGameplayAbility_ComboAttack.generated.h"

UCLASS()
class UBattleGameplayAbility_ComboAttack : public UBattleGameplayAbility
{
	GENERATED_BODY()
public:

	UBattleGameplayAbility_ComboAttack(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPCNotifyHit(const FHitResult& HitResult, float HitCheckTime);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:
	
	FName GetNextSection();
	void StartComboTimer(FName MontageSectionName);
	void CheckComboInput();
	void AllowInput();

	UFUNCTION(Server, Reliable)
	void ServerRPCMontageSectionChanged();
	
	UFUNCTION()
	void SelectHitCheck(const FHitResult HitResult, const float AttackTime);
	UFUNCTION()
	void OnCompleted();
	UFUNCTION()
	void OnInterrupted();

	UFUNCTION()
	void OnRep_AlreadyHitActors();
protected:

	// 0 => 일반공격, 1 => 강공격
	UPROPERTY(EditDefaultsOnly)
	uint8 AttackMode = 0;
	
	UPROPERTY()
	TObjectPtr<UBattleComboData> CurrentComboData;

	UPROPERTY()
	TObjectPtr<UAnimMontage> CurrentComboMontage;

	UPROPERTY(ReplicatedUsing=OnRep_AlreadyHitActors)
	TArray<TObjectPtr<AActor>> AlreadyHitActors;
	
	uint8 CurrentComboIndex = 0;
	FTimerHandle ComboTimerHandle;
	FTimerHandle ComboAllowedTimerHandle;
	bool bAllowedInput;
	bool bHasNextComboInput;

	float AcceptHitDistance = 300.f;

	
};