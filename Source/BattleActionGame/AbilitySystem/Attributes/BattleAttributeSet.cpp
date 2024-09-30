#include "BattleAttributeSet.h"
#include "BattleActionGame/AbilitySystem/BattleAbilitySystemComponent.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleAttributeSet)

UBattleAttributeSet::UBattleAttributeSet()
{
}

UBattleAbilitySystemComponent* UBattleAttributeSet::GetBattleAbilitySystemComponent() const
{
	return Cast<UBattleAbilitySystemComponent>(GetOwningAbilitySystemComponent());
}
