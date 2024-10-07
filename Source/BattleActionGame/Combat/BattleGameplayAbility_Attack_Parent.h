#pragma once

#include "BattleActionGame/AbilitySystem/Abilities/BattleGameplayAbility.h"
#include "BattleGameplayAbility_Attack_Parent.generated.h"

class UBattleComboData;

UCLASS(Abstract)
class UBattleGameplayAbility_Attack_Parent : public UBattleGameplayAbility
{
	GENERATED_BODY()
public:

	UBattleGameplayAbility_Attack_Parent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable, WithValidation)
	virtual void ServerRPCNotifyHit(const FHitResult& HitResult, float HitCheckTime);
	
	virtual void AttackHitConfirm(const FHitResult& HitResult);
	
protected:

	virtual void OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& InData, FGameplayTag ApplicationTag);

	UFUNCTION()
	virtual void SelectHitCheck(const FHitResult HitResult, const float AttackTime);
	UFUNCTION()
	virtual void OnCompleted();
	UFUNCTION()
	virtual void OnInterrupted();

	UFUNCTION()
	virtual void OnRep_AlreadyHitActors();
	
protected:
	UPROPERTY(EditDefaultsOnly)
	uint8 AttackMode = 0;
	
	UPROPERTY()
	TObjectPtr<UBattleComboData> CurrentAttackData;

	UPROPERTY()
	TObjectPtr<UAnimMontage> CurrentAttackMontage;

	UPROPERTY(ReplicatedUsing=OnRep_AlreadyHitActors)
	TArray<TObjectPtr<AActor>> AlreadyHitActors;

	float AcceptHitDistance = 1000.f;
	
	
};

