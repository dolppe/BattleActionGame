#pragma once

#include "BattleGameplayAbility_SpecialAction_Parent.h"
#include "GameplayMessageSubsystem.h"
#include "BattleGameplayAbility_CounterAction.generated.h"


UCLASS()
class UBattleGameplayAbility_CounterAction : public UBattleGameplayAbility_SpecialAction_Parent
{
	GENERATED_BODY()
public:

	UBattleGameplayAbility_CounterAction(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual void MoveToBestSpot() override;
	virtual void PlaySpecialActionMontage() override;

	virtual void StartActionTrigger(FGameplayTag Channel, const FBattleVerbMessage& Notification) override;

	UFUNCTION(Server, Reliable)
	virtual void ServerCounterAction();

	UPROPERTY(EditAnywhere)
	UAnimMontage* CounterMontage;

	UPROPERTY(EditAnywhere)
	FGameplayTag CounterGCN;

	UPROPERTY(EditAnywhere)
	FGameplayTag TargetHitGATag;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> GameplayEffect_Damage;
	
	FGameplayMessageListenerHandle StartListenerHandle;

	UPROPERTY(EditAnywhere)
	float AttackRate = 0.0f;

	UPROPERTY(EditAnywhere)
	float BaseDamage = 0.0f;

	UPROPERTY(EditAnywhere)
	float GroggyValue = 0.0f;
	
	
};
