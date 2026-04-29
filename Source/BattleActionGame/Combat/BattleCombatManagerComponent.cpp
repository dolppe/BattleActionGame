#include "BattleCombatManagerComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AttackCollisionMethod_CircularAOE.h"
#include "AttackCollisionMethod_DirectionalSweep.h"
#include "AttackCollisionMethod_SocketBasedLineTrace.h"
#include "BattleGameplayAbility_ComboAttack.h"
#include "NativeGameplayTags.h"
#include "BattleActionGame/BattleLogChannels.h"
#include "BattleActionGame/Character/BattleCharacterBase.h"
#include "Item/BattleGameplayAbility_UseItem_AttributeBased.h"
#include "Item/BattleItemData.h"
#include "JustGuard/BattleGameplayAbility_JustGuardAttack.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleCombatManagerComponent)

UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_WarnStrong, "GameplayCue.WarnSign.Strong");
UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_WarnJustClash, "GameplayCue.WarnSign.JustClash");
UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_WarnJustGuard, "GameplayCue.WarnSign.JustGuard");



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
	DOREPLIFETIME(ThisClass, CurrentCombo);
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

void UBattleCombatManagerComponent::OnEndAbilityComboGA()
{
	GetWorld()->GetTimerManager().SetTimer(ComboEndTimerHandle, [this]()
	{
		CurrentCombo = nullptr;
	},0.1f,false);
	
}

void UBattleCombatManagerComponent::OnRep_CurrentUsedItemInfo()
{
	
}

UAttackCollisionMethod* UBattleCombatManagerComponent::GetCollisionMethod(ECollisionMethodType CollisionMethod)
{
	return InstancedCollisionMethod[CollisionMethod];
}

void UBattleCombatManagerComponent::OnAttackStart()
{
	if (CurrentAttackGA == nullptr)
	{
		return;
	}
	if (CurrentAttackGA->IsActive())
	{
		CurrentAttackGA->OnAttackStart();
	}
}

void UBattleCombatManagerComponent::OnHitEvent(const FBattleHitMessage& HitMessage)
{
	if (CurrentAttackGA == nullptr)
	{
		return;
	}
	if (CurrentAttackGA->IsActive())
	{
		CurrentAttackGA->ReceivedHits(HitMessage);
	}
}

void UBattleCombatManagerComponent::AllowGuardEvent()
{
	if (CurrentAttackGA == nullptr)
	{
		return;
	}
	if (CurrentAttackGA->IsActive())
	{
		if (UBattleGameplayAbility_JustGuardAttack* JustGuardAttack = Cast<UBattleGameplayAbility_JustGuardAttack>(CurrentAttackGA))
		{
			JustGuardAttack->AllowGuardEvent();
		}
	}
}

void UBattleCombatManagerComponent::TryJustGuard_Implementation(AActor* TryActor)
{
	if (CurrentAttackGA == nullptr)
	{
		return;
	}
	
	if (CurrentAttackGA->IsActive())
	{
		if (UBattleGameplayAbility_JustGuardAttack* JustGuardAttack = Cast<UBattleGameplayAbility_JustGuardAttack>(CurrentAttackGA))
		{
			JustGuardAttack->ReceivedGuard(TryActor);
		}
	}
}

void UBattleCombatManagerComponent::OnAttackWarnSign()
{
	if (CurrentAttackGA == nullptr)
	{
		return;
	}
	if (CurrentAttackGA->IsActive())
	{
		UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
		
		if (UBattleGameplayAbility_JustGuardAttack* JustGuardAttack = Cast<UBattleGameplayAbility_JustGuardAttack>(CurrentAttackGA))
		{
			JustGuardAttack->AllowGuardEvent();
			ASC->ExecuteGameplayCue(TAG_Gameplay_WarnJustGuard);
		}
		else if (CurrentAttackGA->IsAllowedJustClash())
		{
			ASC->ExecuteGameplayCue(TAG_Gameplay_WarnJustClash);
		}
		else if (!CurrentAttackGA->IsAllowedJustDash())
		{
			ASC->ExecuteGameplayCue(TAG_Gameplay_WarnStrong);
		}

		if (ABattleCharacterBase* MyCharacter = Cast<ABattleCharacterBase>(GetOwner()))
		{
			MyCharacter->NetStopMotion(0.1f);
		}
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

void UBattleCombatManagerComponent::OnRep_CurrentCombo()
{
}
