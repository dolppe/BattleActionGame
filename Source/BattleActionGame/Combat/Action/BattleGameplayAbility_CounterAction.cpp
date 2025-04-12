#include "BattleGameplayAbility_CounterAction.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemInterface.h"
#include "GameplayMessageSubsystem.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "BattleActionGame/BattleGameplayTags.h"
#include "BattleActionGame/Combat/BattleCombatManagerComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleGameplayAbility_CounterAction)

UBattleGameplayAbility_CounterAction::UBattleGameplayAbility_CounterAction(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UBattleGameplayAbility_CounterAction::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	UBattleCombatManagerComponent* CombatManagerComponent = Cast<UBattleCombatManagerComponent>(Character->GetComponentByClass(UBattleCombatManagerComponent::StaticClass()));

	if (Character->IsLocallyControlled())
	{
		UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());

		StartListenerHandle = MessageSystem.RegisterListener(FBattleGameplayTags::Get().SpecialAction_Counter_Start, this, &UBattleGameplayAbility_CounterAction::StartActionTrigger);
	}
}

void UBattleGameplayAbility_CounterAction::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	
	if (GetWorld()->GetNetMode() == NM_Client)
	{
		
	}
	else
	{
		
		Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	}

	if (Character->IsLocallyControlled())
	{
		UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());

		MessageSystem.UnregisterListener(StartListenerHandle);
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UBattleGameplayAbility_CounterAction::MoveToBestSpot()
{
	if (GetWorld()->GetNetMode() == NM_Client)
	{
		return;
	}
	if (ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		FVector Direction = TargetActor->GetActorLocation() - Character->GetActorLocation();
		Direction.Z = 0.0f;
		Character->SetActorRotation(Direction.Rotation());
		Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	}
}

void UBattleGameplayAbility_CounterAction::PlaySpecialActionMontage()
{

	UAbilityTask_PlayMontageAndWait* PlayAttackMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("PlayMontage"), CounterMontage, 0.65f);
	PlayAttackMontage->OnCompleted.AddDynamic(this, &UBattleGameplayAbility_CounterAction::OnCompleted);
	PlayAttackMontage->OnInterrupted.AddDynamic(this, &UBattleGameplayAbility_CounterAction::OnInterrupted);
	PlayAttackMontage->OnBlendOut.AddDynamic(this, &UBattleGameplayAbility_CounterAction::OnBlendOut);
	PlayAttackMontage->ReadyForActivation();
	
}


void UBattleGameplayAbility_CounterAction::StartActionTrigger(FGameplayTag Channel,
	const FBattleVerbMessage& Notification)
{
	Super::StartActionTrigger(Channel, Notification);
	
	ServerCounterAction();
	
}

PRAGMA_DISABLE_OPTIMIZATION

void UBattleGameplayAbility_CounterAction::ServerCounterAction_Implementation()
{
	ACharacter* TargetCharacter = Cast<ACharacter>(TargetActor);
	
	if (TargetHitGATag.IsValid())
	{
		FGameplayTagContainer TagContainer;
		TagContainer.AddTag(TargetHitGATag);
		
		UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetCharacter)->TryActivateAbilitiesByTag(TagContainer);
	}
	
	if (GameplayEffect_Damage != nullptr)
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(GameplayEffect_Damage, 1);
	
		if (SpecHandle.IsValid())
		{
			SpecHandle.Data->SetSetByCallerMagnitude(FBattleGameplayTags::Get().GameplayEffect_Data_BaseDamage, BaseDamage);
			SpecHandle.Data->SetSetByCallerMagnitude(FBattleGameplayTags::Get().GameplayEffect_Data_AttackRate, AttackRate);
			SpecHandle.Data->SetSetByCallerMagnitude(FBattleGameplayTags::Get().GameplayEffect_Data_GroggyValue, GroggyValue);
		
			ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, SpecHandle, FGameplayAbilityTargetDataHandle());
		}	
	}
	
	if (CounterGCN.IsValid())
	{
		FGameplayCueParameters Params;
		Params.Instigator = GetAvatarActorFromActorInfo();

		UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();

		ASC->ExecuteGameplayCue(CounterGCN, Params);
	}
	
}

PRAGMA_ENABLE_OPTIMIZATION
