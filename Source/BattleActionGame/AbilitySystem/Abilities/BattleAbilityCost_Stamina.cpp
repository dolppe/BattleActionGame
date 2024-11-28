#include "BattleAbilityCost_Stamina.h"

#include "AbilitySystemComponent.h"
#include "BattleActionGame/AbilitySystem/Attributes/BattleHealthSet.h"
#include "BattleActionGame/Character/BattleCharacterBase.h"
#include "BattleActionGame/Character/BattleHealthComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleAbilityCost_Stamina)

UBattleAbilityCost_Stamina::UBattleAbilityCost_Stamina()
{
}

bool UBattleAbilityCost_Stamina::CheckCost(const UBattleGameplayAbility* Ability,
                                           const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                           FGameplayTagContainer* OptionalRelevantTags) const
{
	if (ABattleCharacterBase* CharacterBase = CastChecked<ABattleCharacterBase>(ActorInfo->AvatarActor))
	{
		if (CharacterBase->GetHealthComponent()->GetStamina() >= StaminaAmount)
		{
			return true;
		}
	}

	return false;
	
}

void UBattleAbilityCost_Stamina::ApplyCost(const UBattleGameplayAbility* Ability,
                                           const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                           const FGameplayAbilityActivationInfo ActivationInfo)
{

	if (ABattleCharacterBase* CharacterBase = CastChecked<ABattleCharacterBase>(ActorInfo->AvatarActor))
	{
		if (UAbilitySystemComponent* ASC = CharacterBase->GetAbilitySystemComponent())
		{
			ASC->ApplyModToAttribute(UBattleHealthSet::GetStaminaAttribute(),EGameplayModOp::Additive, -StaminaAmount);
		}
	}
}
