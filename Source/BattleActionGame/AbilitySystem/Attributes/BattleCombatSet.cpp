#include "BattleCombatSet.h"

#include "GameplayEffectExtension.h"
#include "BattleActionGame/BattleLogChannels.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleCombatSet)

UBattleCombatSet::UBattleCombatSet()
	: AttackPower(20.f)
	, ImpactResistance(0.0f)
{
}


void UBattleCombatSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetImpactDamageAttribute())
	{
		if (Data.EvaluatedData.Magnitude >0.0f)
		{
			if (OnReceivedImpactDamage.IsBound())
			{
				const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
				AActor* Instigator = EffectContext.GetOriginalInstigator();
				AActor* Causer = EffectContext.GetEffectCauser();
				
				OnReceivedImpactDamage.Broadcast(Instigator, Causer, Data.EffectSpec, Data.EvaluatedData.Magnitude);
				SetImpactDamage(0.0f);
			}
		}
	}
}

void UBattleCombatSet::OnRep_AttackPower(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBattleCombatSet, AttackPower, OldValue);
}

void UBattleCombatSet::OnRep_ImpactResistance(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBattleCombatSet, ImpactResistance, OldValue);
}

void UBattleCombatSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UBattleCombatSet, AttackPower, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBattleCombatSet, ImpactResistance, COND_OwnerOnly, REPNOTIFY_Always);
}
