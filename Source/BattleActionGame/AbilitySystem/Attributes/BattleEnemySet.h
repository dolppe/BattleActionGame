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

	mutable FBattleAttributeEvent OnGroggyState;

protected:

	UFUNCTION()
	void OnRep_GroggyValue(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxGroggyValue(const FGameplayAttributeData& OldValue);

	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

private:
	
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_GroggyValue, Category = "Battle|Enemy", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData GroggyValue;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxGroggyValue, Category = "Battle|Enemy", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxGroggyValue;
	
};

