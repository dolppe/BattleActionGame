#include "BattleGameplayAbility_JustGuardAttack.h"

#include "AbilitySystemComponent.h"
#include "BattleGameplayAbility_JustGuard.h"
#include "BattleGameplayAbility_TryJustGuard.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "BattleActionGame/BattleGameplayTags.h"
#include "BattleActionGame/Character/BattleCharacter.h"
#include "BattleActionGame/Combat/BattleCombatManagerComponent.h"
#include "BattleActionGame/Physics/BattleCollisionChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleGameplayAbility_JustGuardAttack)

UBattleGameplayAbility_JustGuardAttack::UBattleGameplayAbility_JustGuardAttack(
	const FObjectInitializer& ObjectInitializer)
{
	AttackType = EAttackType::Basic;
}

void UBattleGameplayAbility_JustGuardAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                             const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                             const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	const ABattleCharacterBase* Character = Cast<ABattleCharacterBase>(ActorInfo->AvatarActor);
	UBattleCombatManagerComponent* CurrentCombatManager = CastChecked<UBattleCombatManagerComponent>(Character->GetComponentByClass(UBattleCombatManagerComponent::StaticClass()));

	AttackData = &CurrentCombatManager->GetAttackData()->BasicAttacks[AttackMode];

	const FName MontageSectionName = *FString::Printf(TEXT("%s%d"), *AttackData->MontageSectionName, 1);

	UAbilityTask_PlayMontageAndWait* PlayAttackMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("PlayMontage"), AttackData->Montage, 0.65f, MontageSectionName);
	PlayAttackMontage->OnCompleted.AddDynamic(this, &UBattleGameplayAbility_JustGuardAttack::OnCompleted);
	PlayAttackMontage->OnInterrupted.AddDynamic(this, &UBattleGameplayAbility_JustGuardAttack::OnInterrupted);
	PlayAttackMontage->OnBlendOut.AddDynamic(this, &UBattleGameplayAbility_JustGuardAttack::OnBlendOut);
	PlayAttackMontage->ReadyForActivation();
}

void UBattleGameplayAbility_JustGuardAttack::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                                        const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                        bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UBattleGameplayAbility_JustGuardAttack::ReceivedHits(const FBattleHitMessage& HitMessage)
{
	if (bAllowGuardEvent)
	{
		PendingHitMessages.Add(HitMessage);
	}
	else
	{
		Super::ReceivedHits(HitMessage);
	}
}

void UBattleGameplayAbility_JustGuardAttack::AllowGuardEvent()
{
	if (GetWorld()->GetNetMode() == NM_Client)
	{
		return;
	}
	bAllowGuardEvent = true;

	TArray<FOverlapResult> OverlapResults;

	GetWorld()->OverlapMultiByChannel(OverlapResults, GetAvatarActorFromActorInfo()->GetActorLocation(), FQuat::Identity, Battle_TraceChannel_AttackToCharacter, FCollisionShape::MakeSphere(AllowGuardRadius));

	for (const FOverlapResult& OverlapResult : OverlapResults)
	{
		if (ABattleCharacter* PlayerCharacter = Cast<ABattleCharacter>(OverlapResult.GetActor()))
		{
			FGameplayAbilitySpec AbilitySpec(UBattleGameplayAbility_TryJustGuard::StaticClass());
			FGameplayEventData GameplayEventData;

			GameplayEventData.Instigator = GetAvatarActorFromActorInfo();
			GameplayEventData.EventMagnitude = AllowGuardEventTime;
			
			PlayerCharacter->GetAbilitySystemComponent()->GiveAbilityAndActivateOnce(AbilitySpec, &GameplayEventData);
		}
	}

	GetWorld()->GetTimerManager().SetTimer(AllowGuardEventHandle, this, &ThisClass::DisallowGuardEvent, AllowGuardEventTime, false);
}

void UBattleGameplayAbility_JustGuardAttack::DisallowGuardEvent()
{
	bAllowGuardEvent = false;


	for (const FBattleHitMessage& HitMessage : PendingHitMessages)
	{
		bHitCritical = false;
		
		if (GetWorld()->GetNetMode() == NM_Client)
		{
			if (AttackIdx != HitMessage.WindowIndex)
			{
				AttackIdx = HitMessage.WindowIndex;
			}
			ServerRPCNotifyHit(HitMessage, HitMessage.HitTime);
		}
		else
		{
			AttackHitConfirm(HitMessage);
		}
	}
	
}

void UBattleGameplayAbility_JustGuardAttack::ReceivedGuard(AActor* TryActor)
{
	if (bAllowGuardEvent)
	{
		if (UBattleCombatManagerComponent* CombatManagerComponent = Cast<UBattleCombatManagerComponent>(GetAvatarActorFromActorInfo()->GetComponentByClass(UBattleCombatManagerComponent::StaticClass())))
		{
			CombatManagerComponent->SetCurrentTargetActor(TryActor);
			
			FGameplayTagContainer GameplayTags;
			GameplayTags.AddTag(FBattleGameplayTags::Get().Ability_Type_Reaction_JustGuard);
			
			if (GetAbilitySystemComponentFromActorInfo()->TryActivateAbilitiesByTag(GameplayTags))
			{
				bAllowGuardEvent = false;
			}
		}
		
	}
}

void UBattleGameplayAbility_JustGuardAttack::OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& InData,
	FGameplayTag ApplicationTag)
{
	Super::OnTargetDataReadyCallback(InData, ApplicationTag);

	OnTargetDataReady(InData);
}
