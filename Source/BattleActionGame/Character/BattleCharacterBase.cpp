#include "BattleCharacterBase.h"

#include "BattleActionGame/AbilitySystem/BattleAbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleCharacterBase)

UBattleAbilitySystemComponent* ABattleCharacterBase::GetBattleAbilitySystemComponent() const
{
	return NewObject<UBattleAbilitySystemComponent>();
}

UAbilitySystemComponent* ABattleCharacterBase::GetAbilitySystemComponent() const
{
	return NewObject<UAbilitySystemComponent>();
}
