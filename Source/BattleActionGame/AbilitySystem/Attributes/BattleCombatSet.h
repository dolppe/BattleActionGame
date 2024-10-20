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

	ATTRIBUTE_ACCESSORS(UBattleCombatSet, BaseDamage);
	ATTRIBUTE_ACCESSORS(UBattleCombatSet, BaseHeal);
	ATTRIBUTE_ACCESSORS(UBattleCombatSet, AttackPower);

protected:

	UFUNCTION()
	void OnRep_BaseDamage(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_BaseHeal(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_AttackPower(const FGameplayAttributeData& OldValue);

private:

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseDamage, Category = "Battle|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData BaseDamage;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseHeal, Category = "Battle|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData BaseHeal;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AttackPower, Category = "Battle|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AttackPower;

	
};

