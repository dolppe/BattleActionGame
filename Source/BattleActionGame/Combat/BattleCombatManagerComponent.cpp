#include "BattleCombatManagerComponent.h"

#include "AbilitySystemComponent.h"
#include "BattleGameplayAbility_ComboAttack.h"
#include "BattleActionGame/Character/BattleCharacterBase.h"
#include "Item/BattleGameplayAbility_UseItem_AttributeBased.h"
#include "Item/BattleItemData.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleCombatManagerComponent)




UBattleCombatManagerComponent::UBattleCombatManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}

void UBattleCombatManagerComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, CurrentUsedItemInfo);
}

void UBattleCombatManagerComponent::BeginPlay()
{
	Super::BeginPlay();
}

PRAGMA_DISABLE_OPTIMIZATION
void UBattleCombatManagerComponent::UseItem(EItemType Item)
{
	GetPawn<ABattleCharacterBase>()->GetAbilitySystemComponent()->TryActivateAbilityByClass(ItemTypeToAbility[Item]);
}

PRAGMA_ENABLE_OPTIMIZATION

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

void UBattleCombatManagerComponent::OnRep_CurrentUsedItemInfo()
{
	
}
