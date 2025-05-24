#pragma once

#include "BattleActionGame/AbilitySystem/Abilities/BattleGameplayAbility.h"
#include "BattleActionGame/Messages/BattleVerbMessage.h"
#include "BattleGameplayAbility_SpecialAction_Parent.generated.h"

UCLASS()
class UBattleGameplayAbility_SpecialAction_Parent : public UBattleGameplayAbility
{
	GENERATED_BODY()

public:

	UBattleGameplayAbility_SpecialAction_Parent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void SetTargetActor(AActor* InTargetActor)
	{
		TargetActor = InTargetActor;
	}
	
protected:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual void MoveToBestSpot();
	virtual void PlaySpecialActionMontage();
	virtual void PlayStartCue();

	virtual void StartActionTrigger(FGameplayTag Channel, const FBattleVerbMessage& Notification);
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnSpecialActionTriggered(FGameplayTag Channel, const FBattleVerbMessage& Notification);

	UFUNCTION()
	virtual void OnCompleted();
	UFUNCTION()
	virtual void OnInterrupted();
	UFUNCTION()
	virtual void OnBlendOut();
	
	UPROPERTY(BlueprintReadWrite, Category="Action")
	TObjectPtr<const AActor> TargetActor;
	
};

