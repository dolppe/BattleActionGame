#include "BattleCombatSet.h"

#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleCombatSet)

UBattleCombatSet::UBattleCombatSet()
	: BaseDamage(0.0f)
	, BaseHeal(0.0f)
{
}

void UBattleCombatSet::OnRep_BaseDamage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBattleCombatSet, BaseDamage, OldValue);
}

void UBattleCombatSet::OnRep_BaseHeal(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBattleCombatSet, BaseHeal, OldValue);
}

void UBattleCombatSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UBattleCombatSet, BaseDamage, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBattleCombatSet, BaseHeal, COND_OwnerOnly, REPNOTIFY_Always);
}
