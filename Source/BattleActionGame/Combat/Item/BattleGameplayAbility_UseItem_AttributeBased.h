#pragma once

#include "BattleActionGame/AbilitySystem/Abilities/BattleGameplayAbility.h"
#include "BattleGameplayAbility_UseItem_AttributeBased.generated.h"

UCLASS(Blueprintable)
class UBattleGameplayAbility_UseItem_AttributeBased : public UBattleGameplayAbility
{
	GENERATED_BODY()

public:

	UBattleGameplayAbility_UseItem_AttributeBased(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:

	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> DurationGameplayEffectClass;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> InstanceGameplayEffectClass;
	
};

