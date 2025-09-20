#include "BattleGameplayAbility_Attack_Parent.h"

#include "AbilitySystemComponent.h"
#include "BattleCombatManagerComponent.h"
#include "BattleActionGame/BattleGameplayTags.h"
#include "BattleActionGame/BattleLogChannels.h"
#include "BattleActionGame/Character/BattleCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleGameplayAbility_Attack_Parent)

UBattleGameplayAbility_Attack_Parent::UBattleGameplayAbility_Attack_Parent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
}

void UBattleGameplayAbility_Attack_Parent::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	const ABattleCharacterBase* Character = Cast<ABattleCharacterBase>(ActorInfo->AvatarActor);
	BA_DEFAULT_LOG(LogBattle,Log,TEXT("Start"));

	AttackIdx = -1;
	AlreadyHitActors.Empty();

	if (Character->IsLocallyControlled())
	{
		UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());

		StartListenerHandle = MessageSystem.RegisterListener(FBattleGameplayTags::Get().Combat_Attack_Hit, this, &UBattleGameplayAbility_Attack_Parent::ReceivedHits);
	}
	if (GetWorld()->GetNetMode() != NM_Client)
	{

		
	}
}

void UBattleGameplayAbility_Attack_Parent::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	ABattleCharacterBase* Character = Cast<ABattleCharacterBase>(ActorInfo->AvatarActor);
	BA_DEFAULT_LOG(LogBattle,Log,TEXT("End"));
	
	if (Character->IsLocallyControlled())
	{
		// 테스크 실행
		UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());

		if (StartListenerHandle.IsValid())
		{
			MessageSystem.UnregisterListener(StartListenerHandle);
		}
		if (EndListenerHandle.IsValid())
		{
			MessageSystem.UnregisterListener(EndListenerHandle);
		}

		UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
		
		if (ASC)
		{
			ASC->RemoveLooseGameplayTag(FBattleGameplayTags::Get().Status_Action_Attack, 3);
			ASC->RemoveLooseGameplayTag(FBattleGameplayTags::Get().Block_Movement, 3);
			ASC->RemoveLooseGameplayTag(FBattleGameplayTags::Get().Block_Movement_AllowRotation, 3);
		}
	}
	if (GetWorld()->GetNetMode() != NM_Client)
	{
		UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
		
		if (ASC)
		{
			ASC->RemoveLooseGameplayTag(FBattleGameplayTags::Get().Status_Action_Attack, 3);
			ASC->RemoveLooseGameplayTag(FBattleGameplayTags::Get().Block_Movement, 3);
			ASC->RemoveLooseGameplayTag(FBattleGameplayTags::Get().Block_Movement_AllowRotation, 3);
		}

	}
	
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UBattleGameplayAbility_Attack_Parent::CancelAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
	
}


void UBattleGameplayAbility_Attack_Parent::AttackHitConfirm(const FBattleHitMessage& HitMessage)
{
	if (GetWorld()->GetNetMode() == NM_Client)
	{
		return;
	}

	if (AttackIdx != HitMessage.WindowIndex)
	{
		AttackIdx = HitMessage.WindowIndex;
		AlreadyHitActors.Empty();
	}

	for (const FHitResult& HitResult : HitMessage.HitResults)
	{
		AActor* HitActor = HitResult.GetActor();
		if (IsValid(HitActor))
		{
			if (AlreadyHitActors.Contains(HitActor))
			{
				continue;
			}

			AlreadyHitActors.Add(HitActor);
			//BA_DEFAULT_LOG(LogBattle, Log, TEXT("%s"), *HitActor->GetName());

			FGameplayAbilityTargetDataHandle TargetData;
			FGameplayAbilityTargetData_SingleTargetHit* NewTargetData = new FGameplayAbilityTargetData_SingleTargetHit();
			NewTargetData->HitResult = HitResult;
			TargetData.Add(NewTargetData);

			if (IAbilitySystemInterface* AbilitySystemObject = Cast<IAbilitySystemInterface>(HitActor))
			{
				if (UAbilitySystemComponent* HitActorASC = AbilitySystemObject->GetAbilitySystemComponent())
				{
					if (bAllowJustClash && HitActorASC->HasMatchingGameplayTag(FBattleGameplayTags::Get().Status_Parry_JustClash))
					{
						
						if (UBattleCombatManagerComponent* CombatManagerComponent = Cast<UBattleCombatManagerComponent>(HitActor->GetComponentByClass(UBattleCombatManagerComponent::StaticClass())))
						{
							CombatManagerComponent->SetCurrentTargetActor(GetAvatarActorFromActorInfo());
						}

						FGameplayEventData Payload;
						Payload.EventTag = FBattleGameplayTags::Get().Ability_Trigger_JustClash;
						Payload.Instigator = GetAvatarActorFromActorInfo();
						Payload.Target = GetAvatarActorFromActorInfo();
						Payload.TargetData = TargetData;

						//BA_DEFAULT_LOG(LogBattle, Log, TEXT("JustClashTag Add"));
						//HitActorASC->AddLooseGameplayTag(FBattleGameplayTags::Get().Ability_Trigger_JustClash);

						HitActorASC->HandleGameplayEvent(Payload.EventTag, &Payload);
						
						bool bReplicatedEndAbility = true;
						bool bWasCancelled = true;
						EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
						return;
					}
					
					if (bAllowJustDash && HitActorASC->HasMatchingGameplayTag(FBattleGameplayTags::Get().Status_Parry_JustDash))
					{
						FGameplayEventData Payload;
						Payload.EventTag = FBattleGameplayTags::Get().GameplayEvent_JustDash;
						Payload.Instigator = GetAvatarActorFromActorInfo();
						Payload.Target = HitActor;
						Payload.TargetData = TargetData;

						if (UBattleCombatManagerComponent* CombatManagerComponent = Cast<UBattleCombatManagerComponent>(HitActor->GetComponentByClass(UBattleCombatManagerComponent::StaticClass())))
						{
							CombatManagerComponent->SetCurrentTargetActor(GetAvatarActorFromActorInfo());
						}
						
						HitActorASC->HandleGameplayEvent(Payload.EventTag, &Payload);
						continue;
					}
				}
			}

			
			
			OnTargetDataReadyCallback(TargetData, FGameplayTag());

		}
	}


}

void UBattleGameplayAbility_Attack_Parent::ServerRPCNotifyHit_Implementation(FBattleHitMessage HitMessage,
                                                                             float HitCheckTime)
{
	TArray<FHitResult> SelectedHitResults;
	
	for (FHitResult& HitResult : HitMessage.HitResults)
	{
		AActor* HitActor = HitResult.GetActor();
		if (IsValid(HitActor))
		{
			const FVector HitLocation = HitResult.Location;
			const FBox HitBox = HitActor->GetComponentsBoundingBox();
			const FVector ActorBoxCenter = (HitBox.Min + HitBox.Max) * 0.5f;
		
			if (FVector::DistSquared(HitLocation, ActorBoxCenter) <= AcceptHitDistance * AcceptHitDistance)
			{
				//UE_LOG(LogTemp, Log, TEXT("Hit Rejected, %f <= %f"), FVector::DistSquared(HitLocation, ActorBoxCenter), AcceptHitDistance * AcceptHitDistance);
				SelectedHitResults.Add(HitResult);
			}
			else
			{
				
			}
			
		}	
	}

	HitMessage.HitResults = SelectedHitResults;

	AttackHitConfirm(HitMessage);
}

bool UBattleGameplayAbility_Attack_Parent::ServerRPCNotifyHit_Validate(FBattleHitMessage HitMessage,
	float HitCheckTime)
{
	return true;
}



void UBattleGameplayAbility_Attack_Parent::ReceivedHits(FGameplayTag Channel, const FBattleHitMessage& HitMessage)
{

	if (GetWorld()->GetNetMode() == NM_Client)
	{ 
		ServerRPCNotifyHit(HitMessage, HitMessage.HitTime);
	}
	else
	{
		AttackHitConfirm(HitMessage);
	}
}

void UBattleGameplayAbility_Attack_Parent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
}

void UBattleGameplayAbility_Attack_Parent::OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& InData,
                                                                     FGameplayTag ApplicationTag)
{
	FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(GameplayEffect_Damage, 1);
	
	if (DamageSpecHandle.IsValid())
	{
		DamageSpecHandle.Data->SetSetByCallerMagnitude(FBattleGameplayTags::Get().GameplayEffect_Data_BaseDamage, AttackData->BaseDamage[AttackIdx]);
		DamageSpecHandle.Data->SetSetByCallerMagnitude(FBattleGameplayTags::Get().GameplayEffect_Data_AttackRate, AttackData->AttackRate[AttackIdx]);
		DamageSpecHandle.Data->SetSetByCallerMagnitude(FBattleGameplayTags::Get().GameplayEffect_Data_GroggyValue, AttackData->GroggyValue[AttackIdx]);
		
		
		ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, DamageSpecHandle, InData);
	}

	if (GetWorld()->GetNetMode() != NM_Client)
	{
		for (TSubclassOf<UGameplayEffect> TargetGE : AttackData->AppliedEffectsToTarget)
		{
			FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(TargetGE, 1);

			ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, SpecHandle, InData);		
		}

		for (TSubclassOf<UGameplayEffect> SelfGE : AttackData->AppliedEffectsToSelf)
		{
			FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(SelfGE, 1);

			ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, SpecHandle);
		}
		

	}
}


void UBattleGameplayAbility_Attack_Parent::OnCompleted()
{
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}

void UBattleGameplayAbility_Attack_Parent::OnInterrupted()
{
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}

void UBattleGameplayAbility_Attack_Parent::OnBlendOut()
{
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}

