#include "BattleGameplayAbility_SpecialAction_VerbMessage.h"
#include "GameFramework/Character.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleGameplayAbility_SpecialAction_VerbMessage)

UBattleGameplayAbility_SpecialAction_VerbMessage::UBattleGameplayAbility_SpecialAction_VerbMessage(
	const FObjectInitializer& ObjectInitializer)
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
}

void UBattleGameplayAbility_SpecialAction_VerbMessage::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                                       const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                                       const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());

	if (Character->IsLocallyControlled())
	{
		UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());

		StartListenerHandle = MessageSystem.RegisterListener(ActionTriggerTag, this, &UBattleGameplayAbility_SpecialAction_VerbMessage::StartActionTrigger);
	}
}

void UBattleGameplayAbility_SpecialAction_VerbMessage::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	
	if (Character->IsLocallyControlled())
	{
		UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());

		MessageSystem.UnregisterListener(StartListenerHandle);
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UBattleGameplayAbility_SpecialAction_VerbMessage::StartActionTrigger(FGameplayTag Channel,
	const FBattleVerbMessage& Notification)
{
	OnSpecialActionTriggered(Channel, Notification);
}
