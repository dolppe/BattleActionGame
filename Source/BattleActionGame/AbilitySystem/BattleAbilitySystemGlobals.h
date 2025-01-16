#pragma once

#include "AbilitySystemGlobals.h"
#include "UObject/UObjectGlobals.h"
#include "BattleAbilitySystemGlobals.generated.h"

struct FGameplayEffectContext;

UCLASS(Config=Game)
class UBattleAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_UCLASS_BODY()

public:
	
	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;

	virtual void PushCurrentAppliedGE(const FGameplayEffectSpec* Spec, UAbilitySystemComponent* AbilitySystemComponent) override;
	virtual void SetCurrentAppliedGE(const FGameplayEffectSpec* Spec) override;
	virtual void PopCurrentAppliedGE() override;
	
	const FGameplayEffectSpec* GetCurrentAppliedGE();

	static UBattleAbilitySystemGlobals& GetBattleGlobals();

private:
	TArray<struct FGameplayEffectSpec> CurrentGameplayEffectSpecStack;
	
};

