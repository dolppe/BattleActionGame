#include "BattleEnemyCharacter.h"
#include "BattleActionGame/AbilitySystem/BattleAbilitySystemComponent.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleEnemyCharacter)

ABattleEnemyCharacter::ABattleEnemyCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UAbilitySystemComponent* ABattleEnemyCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UBattleAbilitySystemComponent* ABattleEnemyCharacter::GetBattleAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
