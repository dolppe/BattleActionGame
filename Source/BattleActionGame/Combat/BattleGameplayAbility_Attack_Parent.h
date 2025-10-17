#pragma once

#include "AttackCollisionMethod.h"
#include "GameplayMessageSubsystem.h"
#include "BattleActionGame/AbilitySystem/Abilities/BattleGameplayAbility.h"
#include "BattleActionGame/Messages/BattleHitMessage.h"
#include "BattleActionGame/Messages/BattleVerbMessage.h"
#include "BattleActionGame/Combat/HitReactionTable.h"
#include "BattleGameplayAbility_Attack_Parent.generated.h"

struct FAttackData;
enum class ECollisionMethodType : uint8;
enum class EAttackType : uint8;




UCLASS(Abstract)
class UBattleGameplayAbility_Attack_Parent : public UBattleGameplayAbility
{
	GENERATED_BODY()
public:

	UBattleGameplayAbility_Attack_Parent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	
	UFUNCTION(Server, Reliable, WithValidation)
	virtual void ServerRPCNotifyHit(FBattleHitMessage HitMessage, float HitCheckTime);
	
	virtual void AttackHitConfirm(const FBattleHitMessage& HitMessage);

	EAttackType GetAttackType() const
	{
		return AttackType;
	}

	UFUNCTION(BlueprintCallable)
	int GetAttackMode() const
	{
		return AttackMode;
	}

	UFUNCTION(BlueprintCallable)
	EStrikeType GetStrikeType(int Index) const;

	UFUNCTION()
	virtual void ReceivedHits(FGameplayTag Channel, const FBattleHitMessage& HitMessage);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:

	virtual void OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& InData, FGameplayTag ApplicationTag);

	UFUNCTION()
	virtual void OnCompleted();
	UFUNCTION()
	virtual void OnInterrupted();
	UFUNCTION()
	virtual void OnBlendOut();
	

	UFUNCTION(BlueprintImplementableEvent)
	void OnAttackStart();
	
protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	uint8 AttackMode = 0;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> GameplayEffect_Damage;

	UPROPERTY(EditAnywhere)
	FGameplayTag HitEffectGCNTag;

	uint8 AttackIdx = -1;

	float AcceptHitDistance = 1500.f;

protected:

	
	FGameplayMessageListenerHandle StartListenerHandle;
	FGameplayMessageListenerHandle EndListenerHandle;

	EAttackType AttackType;
	
	TArray<AActor*> AlreadyHitActors;

	UPROPERTY(EditAnywhere)
	bool bAllowJustDash = true;

	UPROPERTY(EditAnywhere)
	bool bAllowJustClash = false;

	UPROPERTY(EditAnywhere)
	bool bAllowMovement = false;

	const FAttackData* AttackData;

	
};

