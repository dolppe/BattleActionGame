#include "BattleCombatManagerComponent.h"

#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleCombatManagerComponent)

UBattleCombatManagerComponent::UBattleCombatManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

int UBattleCombatManagerComponent::GetCurrentComboIndex()
{
	if (CurrentCombo == nullptr)
	{
		return 1;
	}
	else
	{
		return CurrentCombo->GetComboIndex();
	}
	
}

void UBattleCombatManagerComponent::SetComboGA(UBattleGameplayAbility_ComboAttack* InComboAttack)
{
	CurrentCombo = InComboAttack;		
}
