#pragma once

#include "AbilitySystemComponent.h"
#include "BattleActionGame/AbilitySystem/Attributes/BattleAttributeSet.h"
#include "BattleEnemySet.generated.h"

UCLASS()
class UBattleEnemySet : public UBattleAttributeSet
{
	GENERATED_BODY()
public:

	UBattleEnemySet();

	ATTRIBUTE_ACCESSORS(UBattleEnemySet, GroggyValue);
	ATTRIBUTE_ACCESSORS(UBattleEnemySet, MaxGroggyValue);

	ATTRIBUTE_ACCESSORS(UBattleEnemySet, PoiseValue);
	ATTRIBUTE_ACCESSORS(UBattleEnemySet, MaxPoiseValue);

	mutable FBattleAttributeEvent OnGroggyState;
	mutable FBattleAttributeEvent OnPoiseBreakState;

protected:

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;

private:
	
	
	UPROPERTY(BlueprintReadOnly, Category = "Battle|Enemy", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData GroggyValue;

	UPROPERTY(BlueprintReadOnly, Category = "Battle|Enemy", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxGroggyValue;

	UPROPERTY(BlueprintReadOnly, Category = "Battle|Enemy", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData PoiseValue;

	UPROPERTY(BlueprintReadOnly, Category = "Battle|Enemy", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxPoiseValue;
	
};

