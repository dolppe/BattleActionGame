#include "BattleCombatManagerComponent.h"

#include "AbilitySystemComponent.h"
#include "AttackCollisionMethod_CircularAOE.h"
#include "AttackCollisionMethod_DirectionalSweep.h"
#include "AttackCollisionMethod_SocketBasedLineTrace.h"
#include "BattleGameplayAbility_ComboAttack.h"
#include "BattleActionGame/BattleLogChannels.h"
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
	DOREPLIFETIME(ThisClass, CurrentTargetActor);
	DOREPLIFETIME(ThisClass, AreaCenterData);
	DOREPLIFETIME(ThisClass, LastHitInfo);
}

void UBattleCombatManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	InstancedCollisionMethod.Add(ECollisionMethodType::SocketBasedLineTrace, NewObject<UAttackCollisionMethod_SocketBasedLineTrace>(this));
	InstancedCollisionMethod.Add(ECollisionMethodType::DirectionalSweep, NewObject<UAttackCollisionMethod_DirectionalSweep>(this));
	InstancedCollisionMethod.Add(ECollisionMethodType::CircularAOE, NewObject<UAttackCollisionMethod_CircularAOE>(this));

	for (TTuple<ECollisionMethodType, UAttackCollisionMethod*>& Item : InstancedCollisionMethod)
	{
		Item.Value->SetCharacter(Cast<ACharacter>(GetOwner()));
	}
}

void UBattleCombatManagerComponent::UseItem(EItemType Item)
{
	GetPawn<ABattleCharacterBase>()->GetAbilitySystemComponent()->TryActivateAbilityByClass(ItemTypeToAbility[Item]);
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

void UBattleCombatManagerComponent::OnHitEvent(const FBattleHitMessage& HitMessage)
{
	if (CurrentAttackGA->IsActive())
	{
		CurrentAttackGA->ReceivedHits(HitMessage);
	}
}

void UBattleCombatManagerComponent::OnRep_AreaCenterData()
{
}

void UBattleCombatManagerComponent::OnRep_CurrentTargetActor()
{
	//BA_DEFAULT_LOG(LogBattle,Log,TEXT("CurrentTargetActor Rep"));
}

void UBattleCombatManagerComponent::OnRep_LastHitInfo()
{
}
