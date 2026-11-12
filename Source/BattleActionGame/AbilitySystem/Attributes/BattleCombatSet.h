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
	ATTRIBUTE_ACCESSORS(UBattleCombatSet, TestPower);

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

	UPROPERTY(BlueprintReadOnly, Category = "Battle|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData TestPower;
	
private:
	UPROPERTY(BlueprintReadOnly, Category="Battle|Combat", Meta=(AllowPrivateAccess=true))
	FGameplayAttributeData ImpactDamage;
	
};

UCLASS()
class UBattleTestSet : public UBattleAttributeSet
{
	GENERATED_BODY()
public:
	UBattleTestSet();
	
	ATTRIBUTE_ACCESSORS(UBattleTestSet, TestAttributeA);
	ATTRIBUTE_ACCESSORS(UBattleTestSet, TestAttributeB);
	
private:
	UPROPERTY(BlueprintReadOnly, Category="Battle|Test", Meta=(AllowPrivateAccess=true))
	FGameplayAttributeData TestAttributeA;
	
	UPROPERTY(BlueprintReadOnly, Category="Battle|Test", Meta=(AllowPrivateAccess=true))
	FGameplayAttributeData TestAttributeB;
	
};


