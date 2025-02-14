#include "BattleCombatManagerComponent.h"

#include "AbilitySystemComponent.h"
#include "AttackCollisionMethod_CircularAOE.h"
#include "AttackCollisionMethod_DirectionalSweep.h"
#include "AttackCollisionMethod_SocketBasedLineTrace.h"
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

	InstancedCollisionMethod.Add(ECollisionMethodType::SocketBasedLineTrace, NewObject<UAttackCollisionMethod_SocketBasedLineTrace>(this));
	InstancedCollisionMethod.Add(ECollisionMethodType::DirectionalSweep, NewObject<UAttackCollisionMethod_DirectionalSweep>(this));
	InstancedCollisionMethod.Add(ECollisionMethodType::CircularAOE, NewObject<UAttackCollisionMethod_CircularAOE>(this));
}

PRAGMA_DISABLE_OPTIMIZATION
void UBattleCombatManagerComponent::UseItem(EItemType Item)
{
	GetPawn<ABattleCharacterBase>()->GetAbilitySystemComponent()->TryActivateAbilityByClass(ItemTypeToAbility[Item]);
}

PRAGMA_ENABLE_OPTIMIZATION

UAnimMontage* UBattleCombatManagerComponent::GetAttackMontage(EAttackType AttackType, int Idx) const
{
	switch (AttackType)
	{
	case EAttackType::ComboStrong:
		return CombatData->ComboStrongAttacks[Idx].Montage;
	case EAttackType::Combo:
		return CombatData->ComboAttacks[Idx].Montage;
	case EAttackType::Basic:
		return CombatData->BasicAttacks[Idx].Montage;
	}
	return nullptr;
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

void UBattleCombatManagerComponent::OnRep_CurrentUsedItemInfo()
{
	
}

UAttackCollisionMethod* UBattleCombatManagerComponent::GetCollisionMethod(ECollisionMethodType CollisionMethod)
{
	return InstancedCollisionMethod[CollisionMethod];
}
