#include "BattleGameplayAbility_Attack_Parent.h"

#include "AbilitySystemComponent.h"
#include "BattleCombatManagerComponent.h"
#include "BattleActionGame/BattleGameplayTags.h"
#include "BattleActionGame/BattleLogChannels.h"
#include "BattleActionGame/AbilitySystem/BattleAbilitySystemComponent.h"
#include "BattleActionGame/AbilitySystem/Attributes/BattleEnemySet.h"
#include "BattleActionGame/Character/BattleCharacter.h"
#include "BattleActionGame/Physics/BattleCollisionChannels.h"
#include "BattleActionGame/Physics/BattlePartsManagerComponent.h"
#include "BattleActionGame/Physics/PhysicalMaterialWithTags.h"
#include "BattleActionGame/Player/BattlePlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameStateBase.h"
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
		if (UBattleCombatManagerComponent* CombatManagerComp = Cast<UBattleCombatManagerComponent>(Character->GetComponentByClass(UBattleCombatManagerComponent::StaticClass())))
		{
			CombatManagerComp->SetCurrentAttackGA(this);
		}
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

		UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
		
		if (ASC)
		{
			ASC->SetLooseGameplayTagCount(FBattleGameplayTags::Get().Status_Action_Attack,0);
			ASC->SetLooseGameplayTagCount(FBattleGameplayTags::Get().Block_Movement, 0);
			ASC->SetLooseGameplayTagCount(FBattleGameplayTags::Get().Block_Movement_AllowRotation, 0);

			Character->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		}
	}
	if (GetWorld()->GetNetMode() != NM_Client)
	{
		UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
		
		if (ASC && !Character->IsLocallyControlled())
		{
			ASC->SetLooseGameplayTagCount(FBattleGameplayTags::Get().Status_Action_Attack,0);
			ASC->SetLooseGameplayTagCount(FBattleGameplayTags::Get().Block_Movement, 0);
			ASC->SetLooseGameplayTagCount(FBattleGameplayTags::Get().Block_Movement_AllowRotation, 0);

			Character->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
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


EStrikeType UBattleGameplayAbility_Attack_Parent::GetStrikeType(int Index) const
{
	if (AttackData !=nullptr && AttackData->AttackWindowDatas.Num() != 0 && AttackData->AttackWindowDatas.IsValidIndex(Index))
	{
		return AttackData->AttackWindowDatas[Index].StrikeType;
	}

	return EStrikeType::SwordA;
	
}

void UBattleGameplayAbility_Attack_Parent::ReceivedHits(const FBattleHitMessage& HitMessage)
{
	BA_DEFAULT_LOG(LogBattle, Log, TEXT("AttackIdx => %d"),AttackIdx);
	BA_DEFAULT_LOG(LogBattle, Log, TEXT("HitMessageWindowIdx => %d"),HitMessage.WindowIndex);

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

void UBattleGameplayAbility_Attack_Parent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
}

void UBattleGameplayAbility_Attack_Parent::OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& InData,
                                                                     FGameplayTag ApplicationTag)
{
	const FHitResult* HitResult = InData.Data[0].Get()->GetHitResult();
	const FAttackWindowData* AttackWindowData = &AttackData->AttackWindowDatas[AttackIdx];
	ABattleCharacterBase* AvatarCharacter = Cast<ABattleCharacterBase>(GetAvatarActorFromActorInfo());
	ABattleCharacterBase* TargetCharacter = Cast<ABattleCharacterBase>(HitResult->GetActor());
	
	if (UBattleCombatManagerComponent* TargetCombatManagerComponent = Cast<UBattleCombatManagerComponent>(TargetCharacter->GetComponentByClass(UBattleCombatManagerComponent::StaticClass())))
	{
		FHitInfo HitInfo;
		HitInfo.HitDirection = TargetCharacter->GetActorLocation() - AvatarCharacter->GetActorLocation();
		HitInfo.HitDirection.Z = 0.0f;
		HitInfo.HitTime = GetWorld()->GetGameState()->GetServerWorldTimeSeconds();
		TargetCombatManagerComponent->SetLastHitInfo(HitInfo);
	}
	
	FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(GameplayEffect_Damage, 1);
	
	if (DamageSpecHandle.IsValid())
	{
		DamageSpecHandle.Data->SetSetByCallerMagnitude(FBattleGameplayTags::Get().GameplayEffect_Data_BaseDamage, AttackData->AttackWindowDatas[AttackIdx].BaseDamage);
		DamageSpecHandle.Data->SetSetByCallerMagnitude(FBattleGameplayTags::Get().GameplayEffect_Data_AttackRate, AttackData->AttackWindowDatas[AttackIdx].AttackRate);
		DamageSpecHandle.Data->SetSetByCallerMagnitude(FBattleGameplayTags::Get().GameplayEffect_Data_ImpactPower, AttackData->AttackWindowDatas[AttackIdx].ImpactPower);
		
		
		
		ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, DamageSpecHandle, InData);
	}

	if (GetWorld()->GetNetMode() != NM_Client)
	{
		for (TSubclassOf<UGameplayEffect> TargetGE : AttackData->AttackWindowDatas[AttackIdx].AppliedEffectsToTarget)
		{
			FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(TargetGE, 1);

			ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, SpecHandle, InData);		
		}

		for (TSubclassOf<UGameplayEffect> SelfGE : AttackData->AttackWindowDatas[AttackIdx].AppliedEffectsToSelf)
		{
			FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(SelfGE, 1);

			ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, SpecHandle);
		}
	}


	if (AttackWindowData->HitStopSeconds > 0.0f)
	{
		AvatarCharacter->NetStopMotion(AttackWindowData->HitStopSeconds);
		TargetCharacter->NetStopMotion(AttackWindowData->HitStopSeconds);
	}

	if (const UBattleEnemySet* EnemySet = TargetCharacter->GetBattleAbilitySystemComponent()->GetSet<UBattleEnemySet>())
	{
		if (EnemySet->GetMaxGroggyValue() <= EnemySet->GetGroggyValue() + AttackData->AttackWindowDatas[AttackIdx].ImpactPower)
		{
			bHitCritical = true;
		}
		else if(EnemySet->GetPoiseValue() - AttackData->AttackWindowDatas[AttackIdx].ImpactPower <= 0)
		{
			bHitCritical = true;
		}
	}
	
	
	if (const UPhysicalMaterialWithTags* PhysicalMaterialWithTags = Cast<UPhysicalMaterialWithTags>(HitResult->PhysMaterial.Get()))
	{
		if (UBattlePartsManagerComponent* PartsManagerComponent = Cast<UBattlePartsManagerComponent>(TargetCharacter->GetComponentByClass(UBattlePartsManagerComponent::StaticClass())))
		{
			// GCN에서 Critical한 공격인지에 따른 연출을 하기 위함
			if (PartsManagerComponent->WillPartEventTrigger(PhysicalMaterialWithTags->PartTag))
			{
				bHitCritical = true;
			}
			
			EPhysicalSurface SurfaceType = PartsManagerComponent->GetSurfaceStateFromPart(PhysicalMaterialWithTags->PartTag);

			if (UPhysicalMaterial* DefaultPhysicalMaterial = TargetCharacter->GetPhysicalSurface(SurfaceType))
			{
				FGameplayCueParameters HitGCParams = FGameplayCueParameters();

				HitGCParams.Instigator = GetAvatarActorFromActorInfo();
				HitGCParams.SourceObject = this;
				HitGCParams.Location = HitResult->ImpactPoint;
				HitGCParams.Normal = HitResult->ImpactNormal;
				HitGCParams.PhysicalMaterial = DefaultPhysicalMaterial;
				HitGCParams.RawMagnitude = AttackIdx;

				BA_DEFAULT_LOG(LogBattle, Log, TEXT("PartTag: %s"), *PhysicalMaterialWithTags->PartTag.ToString());


				GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(HitEffectGCNTag, HitGCParams);
			}
		}
		TargetCharacter->HandleDamageToPart(HitResult->BoneName, PhysicalMaterialWithTags->PartTag);
	}

	ABattlePlayerController* BattlePC = GetBattlePlayerControllerFromActorInfo();

	if (bHitCritical)
	{
		if (ABattleCharacter* AvatarPlayerCharacter = Cast<ABattleCharacter>(AvatarCharacter))
		{
			AvatarPlayerCharacter->StartCriticalHit(HitResult->ImpactPoint, TargetCharacter);
					
			if (TSubclassOf<UCameraShakeBase> CameraShake = AvatarPlayerCharacter->GetCriticalCamera())
			{
				BattlePC->ClientStartCameraShake(CameraShake, 1.0f);
			}
		}
	}
	else if (AttackWindowData->CameraShakeClass != nullptr)
	{
		BattlePC->ClientStartCameraShake(AttackWindowData->CameraShakeClass, AttackWindowData->CameraShakeScale);
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

