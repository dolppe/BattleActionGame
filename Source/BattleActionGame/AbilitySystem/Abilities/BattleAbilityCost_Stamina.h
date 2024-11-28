#pragma once

#include "BattleAbilityCost.h"
#include "BattleAbilityCost_Stamina.generated.h"

UCLASS()
class UBattleAbilityCost_Stamina : public UBattleAbilityCost
{
	GENERATED_BODY()
public:

	UBattleAbilityCost_Stamina();

	virtual bool CheckCost(const UBattleGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const override;

	virtual void ApplyCost(const UBattleGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	UPROPERTY(EditAnywhere)
	uint8 StaminaAmount = 0;
	
};

