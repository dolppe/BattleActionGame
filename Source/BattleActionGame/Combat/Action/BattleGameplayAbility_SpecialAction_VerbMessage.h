#pragma once

#include "GameplayMessageSubsystem.h"
#include "BattleActionGame/AbilitySystem/Abilities/BattleGameplayAbility.h"
#include "BattleGameplayAbility_SpecialAction_VerbMessage.generated.h"

UCLASS()
class UBattleGameplayAbility_SpecialAction_VerbMessage : public UBattleGameplayAbility
{
	GENERATED_BODY()
public:

	UBattleGameplayAbility_SpecialAction_VerbMessage(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	virtual void StartActionTrigger(FGameplayTag Channel, const FBattleVerbMessage& Notification);

	UFUNCTION(BlueprintImplementableEvent)
	void OnSpecialActionTriggered(FGameplayTag Channel, const FBattleVerbMessage& Notification);

	UPROPERTY(EditAnywhere)
	FGameplayTag ActionTriggerTag;

private:
	
	FGameplayMessageListenerHandle StartListenerHandle;
	
};

