#pragma once

#include "BattleActionGame/AbilitySystem/Abilities/BattleGameplayAbility.h"
#include "BattleGameplayAbility_Attack.generated.h"

class UBattleComboData;

UCLASS()
class UBattleGameplayAbility_Attack : public UBattleGameplayAbility
{
	GENERATED_BODY()
public:

	UBattleGameplayAbility_Attack(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPCNotifyHit(const FHitResult& HitResult, float HitCheckTime);
	
protected:

	UFUNCTION()
	void SelectHitCheck(const FHitResult HitResult, const float AttackTime);
	UFUNCTION()
	void OnCompleted();
	UFUNCTION()
	void OnInterrupted();

	UFUNCTION()
	void OnRep_AlreadyHitActors();
protected:

	UPROPERTY(EditDefaultsOnly)
	uint8 AttackMode = 0;
	
	UPROPERTY()
	TObjectPtr<UBattleComboData> CurrentAttackData;

	UPROPERTY()
	TObjectPtr<UAnimMontage> CurrentAttackMontage;

	UPROPERTY(ReplicatedUsing=OnRep_AlreadyHitActors)
	TArray<TObjectPtr<AActor>> AlreadyHitActors;

	float AcceptHitDistance = 300.f;
	
};

