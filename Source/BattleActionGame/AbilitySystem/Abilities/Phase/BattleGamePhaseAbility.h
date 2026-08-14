#pragma once

#include "BattleActionGame/AbilitySystem/Abilities/BattleGameplayAbility.h"
#include "BattleGamePhaseAbility.generated.h"

UCLASS()
class UBattleGamePhaseAbility : public UBattleGameplayAbility
{
	GENERATED_BODY()
public:

	UBattleGamePhaseAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) override;

	const FGameplayTag& GetGamePhaseTag() const {return GamePhaseTag;}

protected:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Battle|Game Phase")
	FGameplayTag GamePhaseTag;
	
	
};

