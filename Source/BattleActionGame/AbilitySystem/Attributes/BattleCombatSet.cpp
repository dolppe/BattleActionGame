#include "BattleCombatSet.h"

#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleCombatSet)

UBattleCombatSet::UBattleCombatSet()
	: AttackPower(20.f)
{
}



void UBattleCombatSet::OnRep_AttackPower(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UBattleCombatSet, AttackPower, OldValue);
}

void UBattleCombatSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UBattleCombatSet, AttackPower, COND_OwnerOnly, REPNOTIFY_Always);
}
