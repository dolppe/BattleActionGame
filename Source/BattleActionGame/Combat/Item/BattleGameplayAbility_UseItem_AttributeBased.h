#pragma once

#include "GameplayMessageSubsystem.h"
#include "BattleActionGame/AbilitySystem/Abilities/BattleGameplayAbility.h"
#include "BattleGameplayAbility_UseItem_AttributeBased.generated.h"

struct FBattleVerbMessage;

UCLASS(Blueprintable)
class UBattleGameplayAbility_UseItem_AttributeBased : public UBattleGameplayAbility
{
	GENERATED_BODY()

public:

	UBattleGameplayAbility_UseItem_AttributeBased(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:

	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> DurationGameplayEffectClass;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> InstanceGameplayEffectClass;

	UFUNCTION(BlueprintImplementableEvent)
	void OnUsedItem();

	UFUNCTION(Server, Reliable)
	void ServerRPCNotifyUseItem();

	void UseItem();

	UFUNCTION()
	void StartUseItem(FGameplayTag Channel, const FBattleVerbMessage& Notification);

	UFUNCTION()
	void OnCompleted();

	UFUNCTION()
	void OnBlendOut();

	UFUNCTION()
	void OnInterrupted();

protected:

	int Quantity = 1;
	FGameplayMessageListenerHandle UseItemHandle;
	
};

