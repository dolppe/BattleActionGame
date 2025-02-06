#pragma once

#include "GameplayMessageSubsystem.h"
#include "BattleActionGame/AbilitySystem/Abilities/BattleGameplayAbility.h"
#include "BattleActionGame/Messages/BattleVerbMessage.h"
#include "BattleGameplayAbility_Attack_Parent.generated.h"

enum class EAttackType : uint8;
class UBattleCombatData;

UCLASS(Abstract)
class UBattleGameplayAbility_Attack_Parent : public UBattleGameplayAbility
{
	GENERATED_BODY()
public:

	UBattleGameplayAbility_Attack_Parent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(Server, Reliable, WithValidation)
	virtual void ServerRPCNotifyHit(const FHitResult& HitResult, float HitCheckTime);
	
	virtual void AttackHitConfirm(const FHitResult& HitResult);

	EAttackType GetAttackType() const
	{
		return AttackType;
	}
	
protected:

	virtual void OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& InData, FGameplayTag ApplicationTag);

	virtual void StartHitCheck(FGameplayTag Channel, const FBattleVerbMessage& Notification);

	virtual void EndHitCheck(FGameplayTag Channel, const FBattleVerbMessage& Notification);

	UFUNCTION()
	virtual void SelectHitCheck(const FHitResult HitResult, const float AttackTime);
	UFUNCTION()
	virtual void OnCompleted();
	UFUNCTION()
	virtual void OnInterrupted();
	UFUNCTION()
	virtual void OnBlendOut();

	UFUNCTION()
	virtual void OnRep_AlreadyHitActors();

	UFUNCTION(BlueprintImplementableEvent)
	void OnAttackStart();
	
protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	uint8 AttackMode = 0;

	UPROPERTY()
	TObjectPtr<UAnimMontage> CurrentAttackMontage;

	UPROPERTY(ReplicatedUsing=OnRep_AlreadyHitActors)
	TArray<TObjectPtr<AActor>> AlreadyHitActors;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> GameplayEffect_Damage;

	float AcceptHitDistance = 1500.f;
	float AttackRate = 1.0f;
	float GroggyValue = 1.0f; 

protected:
	
	FGameplayMessageListenerHandle StartListenerHandle;
	FGameplayMessageListenerHandle EndListenerHandle;

	EAttackType AttackType;
	
};

