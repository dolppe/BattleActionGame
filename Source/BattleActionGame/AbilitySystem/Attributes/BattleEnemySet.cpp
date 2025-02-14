#include "BattleEnemySet.h"

#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleEnemySet)

UBattleEnemySet::UBattleEnemySet()
	: GroggyValue(0.0f)
	, MaxGroggyValue(100.0f)
{
}


void UBattleEnemySet::OnRep_GroggyValue(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBattleEnemySet, GroggyValue, OldValue);
}

void UBattleEnemySet::OnRep_MaxGroggyValue(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBattleEnemySet, MaxGroggyValue, OldValue);
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
}


void UBattleEnemySet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UBattleEnemySet, GroggyValue, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBattleEnemySet, MaxGroggyValue, COND_OwnerOnly, REPNOTIFY_Always);
}
