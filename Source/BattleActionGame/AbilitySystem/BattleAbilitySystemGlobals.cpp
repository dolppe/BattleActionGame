#include "BattleAbilitySystemGlobals.h"
#include "GameplayEffect.h"
#include "BattleGameplayEffectContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleAbilitySystemGlobals)

UBattleAbilitySystemGlobals::UBattleAbilitySystemGlobals(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

FGameplayEffectContext* UBattleAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FBattleGameplayEffectContext();
}

void UBattleAbilitySystemGlobals::PushCurrentAppliedGE(const FGameplayEffectSpec* Spec,
	UAbilitySystemComponent* AbilitySystemComponent)
{
	check(IsInGameThread());
	CurrentGameplayEffectSpecStack.Push(*Spec);
}

void UBattleAbilitySystemGlobals::SetCurrentAppliedGE(const FGameplayEffectSpec* Spec)
{
	check(IsInGameThread());
	check(CurrentGameplayEffectSpecStack.Num() > 0);

	FGameplayEffectSpec& CurrentSpec = CurrentGameplayEffectSpecStack.Top();
	CurrentSpec = *Spec;
}

void UBattleAbilitySystemGlobals::PopCurrentAppliedGE()
{
	check(IsInGameThread());
	CurrentGameplayEffectSpecStack.Pop();
}

const FGameplayEffectSpec* UBattleAbilitySystemGlobals::GetCurrentAppliedGE()
{
	check(IsInGameThread());

	FGameplayEffectSpec* Spec = nullptr;
	if (CurrentGameplayEffectSpecStack.Num() > 0)
	{
		Spec = &CurrentGameplayEffectSpecStack.Top();
	}

	return Spec;
}

UBattleAbilitySystemGlobals& UBattleAbilitySystemGlobals::GetBattleGlobals()
{
	return *CastChecked<UBattleAbilitySystemGlobals>(IGameplayAbilitiesModule::Get().GetAbilitySystemGlobals());
}
