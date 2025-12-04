#pragma once

#include "BattleActionGame/Combat/BattleGameplayAbility_Attack_Parent.h"
#include "BattleGameplayAbility_JustGuardAttack.generated.h"


// Enemy 전용 공격임
UCLASS()
class UBattleGameplayAbility_JustGuardAttack : public UBattleGameplayAbility_Attack_Parent
{
	GENERATED_BODY()
public:

	UBattleGameplayAbility_JustGuardAttack(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual void ReceivedHits(const FBattleHitMessage& HitMessage) override;

	UFUNCTION()
	void AllowGuardEvent();

	UFUNCTION()
	void DisallowGuardEvent();
	
	void ReceivedGuard(AActor* TryActor);

	virtual void OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& InData, FGameplayTag ApplicationTag) override;
	

protected:

	UFUNCTION(BlueprintImplementableEvent)
	void OnTargetDataReady(const FGameplayAbilityTargetDataHandle& TargetData);


	bool bAllowGuardEvent = false;
	float AllowGuardEventTime = 0.5f;
	FTimerHandle AllowGuardEventHandle;
	
	UPROPERTY(EditAnywhere)
	float AllowGuardRadius = 300.f;
	
private:
	
	TArray<FBattleHitMessage> PendingHitMessages;

	
	
	
};

