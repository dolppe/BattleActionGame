#pragma once

#include "BattleGameplayAbility_Attack_Parent.h"
#include "GameplayMessageSubsystem.h"
#include "GameplayTagContainer.h"
#include "BattleGameplayAbility_HitCheckAttack.generated.h"


struct FHitCheckAttack;
struct FBattleVerbMessage;
struct FGameplayMessageListenerHandle;

UCLASS()
class UBattleGameplayAbility_HitCheckAttack : public UBattleGameplayAbility_Attack_Parent
{
	GENERATED_BODY()
public:

	UBattleGameplayAbility_HitCheckAttack(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void ServerRPCNotifyHit_Implementation(const FHitResult& HitResult, float HitCheckTime) override;

	virtual void AttackHitConfirm(const FHitResult& HitResult) override;


protected:

	virtual void OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& InData, FGameplayTag ApplicationTag) override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnTargetDataReady(const FGameplayAbilityTargetDataHandle& TargetData);

	virtual void SelectHitCheck(const FHitResult HitResult, const float AttackTime) override;

	virtual void OnCompleted() override;
	virtual void OnInterrupted() override;

	virtual void OnRep_AlreadyHitActors() override;

	void AttackEvent(FGameplayTag Channel, const FBattleVerbMessage& Notification);

	
private:

	FGameplayMessageListenerHandle ListenerHandle;

	TObjectPtr<FHitCheckAttack> CurrentHitCheckData;
	
};
