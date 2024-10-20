#include "BattleGameplayAbility_Attack_Parent.h"

#include "AbilitySystemComponent.h"
#include "BattleActionGame/BattleGameplayTags.h"
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

	if (GetWorld()->GetNetMode() == NM_Client)
	{
		
	}
	else
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
	// 실제 데미지 처리, 서버에서 바로 호출되는 경우 거리에 따른 Hit 검사를 안함.

	AActor* HitActor = HitResult.GetActor();
	if (IsValid(HitActor))
	{
		UE_LOG(LogTemp, Log, TEXT("Hit Success => Damage"));
		AlreadyHitActors.Add(HitActor);

		FGameplayAbilityTargetDataHandle TargetData;
		FGameplayAbilityTargetData_SingleTargetHit* NewTargetData = new FGameplayAbilityTargetData_SingleTargetHit();
		NewTargetData->HitResult = HitResult;
		TargetData.Add(NewTargetData);
				
		OnTargetDataReadyCallback(TargetData, FGameplayTag());

	}
	
}

void UBattleGameplayAbility_Attack_Parent::OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& InData,
	FGameplayTag ApplicationTag)
{
	FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(GameplayEffect_Damage, 1);
	
	if (SpecHandle.IsValid())
	{
		SpecHandle.Data->SetSetByCallerMagnitude(FBattleGameplayTags::Get().GameplayEffect_Data_AttackRate, AttackRate);
		
		ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, SpecHandle, InData);
	}
	
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

void UBattleGameplayAbility_Attack_Parent::OnRep_AlreadyHitActors()
{
}
