#pragma once

#include "GameplayMessageSubsystem.h"
#include "BattleActionGame/Combat/Action/BattleGameplayAbility_TargetedAction_Parent.h"
#include "BattleGameplayAbility_JustGuard.generated.h"

UCLASS()
class UBattleGameplayAbility_JustGuard : public UBattleGameplayAbility_TargetedAction_Parent
{
	GENERATED_BODY()
public:

	UBattleGameplayAbility_JustGuard(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual void MoveToBestSpot() override;
	virtual void PlaySpecialActionMontage() override;
	virtual void PlayStartCue() override;

	virtual void StartActionTrigger(FGameplayTag Channel, const FBattleVerbMessage& Notification) override;

	UFUNCTION()
	void OnHitStopEnd();

	UPROPERTY(EditAnywhere)
	UAnimMontage* PlayerMontage;

	UPROPERTY(EditAnywhere)
	UAnimMontage* EnemyMontage;

	UPROPERTY(EditAnywhere)
	FGameplayTag JustGuardImpactGCN;

	UPROPERTY(EditAnywhere)
	FName JustGuardEffectSocketName;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UBattleCameraMode> ImpactCameraMode;

	UPROPERTY(EditAnywhere)
	float HitStopTime = 0.8f;
	
	FGameplayMessageListenerHandle StartListenerHandle;
	FTimerHandle HitStopEndHandle;
	
};

