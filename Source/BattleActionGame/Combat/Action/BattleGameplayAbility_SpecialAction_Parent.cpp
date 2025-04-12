#include "BattleGameplayAbility_SpecialAction_Parent.h"

#include "BattleActionGame/Combat/BattleCombatManagerComponent.h"
#include "GameFramework/Character.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleGameplayAbility_SpecialAction_Parent)

UBattleGameplayAbility_SpecialAction_Parent::UBattleGameplayAbility_SpecialAction_Parent(
	const FObjectInitializer& ObjectInitializer)
		: Super(ObjectInitializer)
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
}

void UBattleGameplayAbility_SpecialAction_Parent::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	
	UBattleCombatManagerComponent* CombatManagerComponent = Cast<UBattleCombatManagerComponent>(Character->GetComponentByClass(UBattleCombatManagerComponent::StaticClass()));

	if (CombatManagerComponent == nullptr)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	if (CombatManagerComponent->GetCurrentTargetActor() == nullptr)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	TargetActor = CombatManagerComponent->GetCurrentTargetActor();

	MoveToBestSpot();

	PlaySpecialActionMontage();

	PlayStartCue();
}

void UBattleGameplayAbility_SpecialAction_Parent::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UBattleGameplayAbility_SpecialAction_Parent::MoveToBestSpot()
{
}

void UBattleGameplayAbility_SpecialAction_Parent::PlaySpecialActionMontage()
{
}

void UBattleGameplayAbility_SpecialAction_Parent::PlayStartCue()
{
}

void UBattleGameplayAbility_SpecialAction_Parent::StartActionTrigger(FGameplayTag Channel,
	const FBattleVerbMessage& Notification)
{
	OnSpecialActionTriggered(Channel, Notification);
	
	
}

void UBattleGameplayAbility_SpecialAction_Parent::OnCompleted()
{
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}

void UBattleGameplayAbility_SpecialAction_Parent::OnInterrupted()
{
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}

void UBattleGameplayAbility_SpecialAction_Parent::OnBlendOut()
{
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}
