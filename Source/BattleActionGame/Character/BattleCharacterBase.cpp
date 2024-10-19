#include "BattleCharacterBase.h"

#include "BattleActionGame/AbilitySystem/BattleAbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleCharacterBase)

ABattleCharacterBase::ABattleCharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UBattleAbilitySystemComponent* ABattleCharacterBase::GetBattleAbilitySystemComponent() const
{
	return NewObject<UBattleAbilitySystemComponent>();
}

UAbilitySystemComponent* ABattleCharacterBase::GetAbilitySystemComponent() const
{
	return NewObject<UAbilitySystemComponent>();
}

UBattleHealthComponent* ABattleCharacterBase::GetHealthComponent() const
{
	return nullptr;
}
