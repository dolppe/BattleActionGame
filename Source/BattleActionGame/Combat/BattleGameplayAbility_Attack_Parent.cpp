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

	if (GetWorld()->GetNetMode() != NM_Client)
	{
		UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
		
		if (ASC)
		{
			ASC->AddLooseGameplayTag(FBattleGameplayTags::Get().Status_Attack_Attacking);
		}
	}
}

void UBattleGameplayAbility_Attack_Parent::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	ABattleCharacterBase* Character = Cast<ABattleCharacterBase>(ActorInfo->AvatarActor);
	
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
	}
	if (GetWorld()->GetNetMode() != NM_Client)
	{
		AlreadyHitActors.Reset();
		
		UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
		
		if (ASC)
		{
			ASC->RemoveLooseGameplayTag(FBattleGameplayTags::Get().Status_Attack_Attacking);
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

void UBattleGameplayAbility_Attack_Parent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UBattleGameplayAbility_Attack_Parent, AlreadyHitActors);
}

void UBattleGameplayAbility_Attack_Parent::ServerRPCNotifyHit_Implementation(const FHitResult& HitResult,
	float HitCheckTime)
{
	AActor* HitActor = HitResult.GetActor();
	if (IsValid(HitActor))
	{
		if (!AlreadyHitActors.Contains(HitActor))
		{
			const FVector HitLocation = HitResult.Location;
			const FBox HitBox = HitActor->GetComponentsBoundingBox();
			const FVector ActorBoxCenter = (HitBox.Min + HitBox.Max) * 0.5f;
			
			if (FVector::DistSquared(HitLocation, ActorBoxCenter) <= AcceptHitDistance * AcceptHitDistance)
			{
				AttackHitConfirm(HitResult);

			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("Hit Rejected, %f <= %f"), FVector::DistSquared(HitLocation, ActorBoxCenter), AcceptHitDistance * AcceptHitDistance);
			}
		}
	}
}

bool UBattleGameplayAbility_Attack_Parent::ServerRPCNotifyHit_Validate(const FHitResult& HitResult, float HitCheckTime)
{
	return true;
}

void UBattleGameplayAbility_Attack_Parent::AttackHitConfirm(const FHitResult& HitResult)
{
	if (GetWorld()->GetNetMode() == NM_Client)
	{
		return;
	}

	AActor* HitActor = HitResult.GetActor();
	if (IsValid(HitActor))
	{
		AlreadyHitActors.Add(HitActor);

		FGameplayAbilityTargetDataHandle TargetData;
		FGameplayAbilityTargetData_SingleTargetHit* NewTargetData = new FGameplayAbilityTargetData_SingleTargetHit();
		NewTargetData->HitResult = HitResult;
		TargetData.Add(NewTargetData);

		if (IAbilitySystemInterface* AbilitySystemObject = Cast<IAbilitySystemInterface>(HitActor))
		{
			if (UAbilitySystemComponent* ASC = AbilitySystemObject->GetAbilitySystemComponent())
			{
				if (ASC->HasMatchingGameplayTag(FBattleGameplayTags::Get().Status_Parry))
				{
					FGameplayEventData Payload;
					Payload.EventTag = FBattleGameplayTags::Get().GameplayEvent_JustDash;
					Payload.Instigator = GetAvatarActorFromActorInfo();
					Payload.Target = HitActor;
					Payload.TargetData = TargetData;

					ASC->HandleGameplayEvent(Payload.EventTag, &Payload);
					return;
				}
			}
		}
		
		OnTargetDataReadyCallback(TargetData, FGameplayTag());

	}
	
}

void UBattleGameplayAbility_Attack_Parent::OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& InData,
	FGameplayTag ApplicationTag)
{
	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(GameplayEffect_Damage, 1);
	
	if (SpecHandle.IsValid())
	{
		SpecHandle.Data->SetSetByCallerMagnitude(FBattleGameplayTags::Get().GameplayEffect_Data_BaseDamage, BaseDamage);
		SpecHandle.Data->SetSetByCallerMagnitude(FBattleGameplayTags::Get().GameplayEffect_Data_AttackRate, AttackRate);
		SpecHandle.Data->SetSetByCallerMagnitude(FBattleGameplayTags::Get().GameplayEffect_Data_GroggyValue, GroggyValue);
		
		
		ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, SpecHandle, InData);
	}
	
}

void UBattleGameplayAbility_Attack_Parent::StartHitCheck(FGameplayTag Channel, const FBattleVerbMessage& Notification)
{
	OnAttackStart();
}

void UBattleGameplayAbility_Attack_Parent::EndHitCheck(FGameplayTag Channel, const FBattleVerbMessage& Notification)
{
}


void UBattleGameplayAbility_Attack_Parent::SelectHitCheck(const FHitResult HitResult, const float AttackTime)
{
	if (!AlreadyHitActors.Contains(HitResult.GetActor()))
	{
		if (GetWorld()->GetNetMode() == NM_Client)
		{
			ServerRPCNotifyHit(HitResult, AttackTime);
		}
		else
		{
			AttackHitConfirm(HitResult);
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

void UBattleGameplayAbility_Attack_Parent::OnRep_AlreadyHitActors()
{
}
