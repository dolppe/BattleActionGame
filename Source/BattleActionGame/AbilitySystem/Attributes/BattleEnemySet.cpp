#include "BattleEnemySet.h"

#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleEnemySet)

UBattleEnemySet::UBattleEnemySet()
	: GroggyValue(0.0f)
	, MaxGroggyValue(100.0f)
	, PoiseValue(100.0f)
	, MaxPoiseValue(100.0f)
{
}


void UBattleEnemySet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}

void UBattleEnemySet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}

void UBattleEnemySet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (GetGroggyValue() >= GetMaxGroggyValue())
	{
		if (OnGroggyState.IsBound())
		{
			const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
			AActor* Instigator = EffectContext.GetOriginalInstigator();
			AActor* Causer = EffectContext.GetEffectCauser();

			OnGroggyState.Broadcast(Instigator, Causer, Data.EffectSpec, GetGroggyValue());
			SetGroggyValue(0.0f);
		}
	}

	if (GetPoiseValue() >= GetMaxPoiseValue())
	{
		SetPoiseValue(GetMaxPoiseValue());
	}
	if (GetPoiseValue() <= 0.0f)
	{
		if (OnPoiseBreakState.IsBound())
		{
			const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
			AActor* Instigator = EffectContext.GetOriginalInstigator();
			AActor* Causer = EffectContext.GetEffectCauser();

			OnPoiseBreakState.Broadcast(Instigator, Causer, Data.EffectSpec, 0.0f);
			SetPoiseValue(GetMaxPoiseValue());
		}
	}


	
}

void UBattleEnemySet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetGroggyValueAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxGroggyValue());		
	}
	else if (Attribute == GetMaxGroggyValueAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.0f);
	}
	else if (Attribute == GetPoiseValueAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxPoiseValue());
	}
	else if (Attribute == GetMaxPoiseValueAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.0f);
	}
	
}



