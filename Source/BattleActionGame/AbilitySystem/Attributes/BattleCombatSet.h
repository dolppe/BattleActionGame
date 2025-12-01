#pragma once

#include "AbilitySystemComponent.h"
#include "BattleActionGame/AbilitySystem/Attributes/BattleAttributeSet.h"
#include "BattleCombatSet.generated.h"

UCLASS()
class UBattleCombatSet : public UBattleAttributeSet
{
	GENERATED_BODY()
public:

	UBattleCombatSet();
	
	ATTRIBUTE_ACCESSORS(UBattleCombatSet, AttackPower);
	ATTRIBUTE_ACCESSORS(UBattleCombatSet, ImpactResistance);
	ATTRIBUTE_ACCESSORS(UBattleCombatSet, ImpactDamage);

	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	mutable FBattleAttributeEvent OnReceivedImpactDamage;

protected:

	UFUNCTION()
	void OnRep_AttackPower(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_ImpactResistance(const FGameplayAttributeData& OldValue);
	
private:

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AttackPower, Category = "Battle|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AttackPower;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ImpactResistance, Category = "Battle|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData ImpactResistance;
	
private:
	UPROPERTY(BlueprintReadOnly, Category="Battle|Combat", Meta=(AllowPrivateAccess=true))
	FGameplayAttributeData ImpactDamage;
	
};

