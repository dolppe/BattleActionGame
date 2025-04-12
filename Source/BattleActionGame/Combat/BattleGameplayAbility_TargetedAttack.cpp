#include "BattleGameplayAbility_TargetedAttack.h"

#include "AttackCollisionMethod_CircularAOE.h"
#include "BattleCombatData.h"
#include "BattleCombatManagerComponent.h"
#include "BattleGameplayAbility_BasicAttack.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "BattleActionGame/BattleGameplayTags.h"
#include "BattleActionGame/Character/BattleCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleGameplayAbility_TargetedAttack)

UBattleGameplayAbility_TargetedAttack::UBattleGameplayAbility_TargetedAttack(
	const FObjectInitializer& ObjectInitializer)
{
	AttackType = EAttackType::Targeted;
}

void UBattleGameplayAbility_TargetedAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	const ABattleCharacterBase* Character = Cast<ABattleCharacterBase>(ActorInfo->AvatarActor);
	UBattleCombatManagerComponent* CurrentCombatManager = CastChecked<UBattleCombatManagerComponent>(Character->GetComponentByClass(UBattleCombatManagerComponent::StaticClass()));

	CurrentAttackData = &CurrentCombatManager->GetAttackData()->TargetedAttacks[AttackMode];
	CurrentAttackMontage = CurrentAttackData->Montage;

	BaseDamage = CurrentAttackData->BaseDamage;
	AttackRate = CurrentAttackData->AttackRate;
	GroggyValue = CurrentAttackData->GroggyValue;
	UAttackCollisionData_CircularAOE* CollisionMethod_CircularAoe = Cast<UAttackCollisionData_CircularAOE>(CurrentAttackData->CollisionMethod);
	AttackRadius = CollisionMethod_CircularAoe->AttackRadius;

	const FName MontageSectionName = *FString::Printf(TEXT("%s%d"), *CurrentAttackData->MontageSectionName, 1);

	UAbilityTask_PlayMontageAndWait* PlayAttackMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("PlayMontage"), CurrentAttackMontage, 0.65f, MontageSectionName);
	PlayAttackMontage->OnCompleted.AddDynamic(this, &UBattleGameplayAbility_TargetedAttack::OnCompleted);
	PlayAttackMontage->OnInterrupted.AddDynamic(this, &UBattleGameplayAbility_TargetedAttack::OnInterrupted);
	PlayAttackMontage->OnBlendOut.AddDynamic(this, &UBattleGameplayAbility_TargetedAttack::OnBlendOut);
	PlayAttackMontage->ReadyForActivation();

	if (Character->IsLocallyControlled())
	{
		UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());

		StartListenerHandle = MessageSystem.RegisterListener(FBattleGameplayTags::Get().Combat_Attack_Event_Start, this, &UBattleGameplayAbility_TargetedAttack::StartHitCheck);
	}
	if (GetWorld()->GetNetMode() != NM_Client)
	{
		Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
		AlreadyHitActors.Reset();
	}
	
}

void UBattleGameplayAbility_TargetedAttack::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	ABattleCharacterBase* Character = Cast<ABattleCharacterBase>(ActorInfo->AvatarActor);
	
	if (Character->IsLocallyControlled())
	{
		// 테스크 실행
		UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());

		MessageSystem.UnregisterListener(StartListenerHandle);
	}
	if (GetWorld()->GetNetMode() != NM_Client)
	{
		// 몽타주 멀티캐스트 필요
		Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UBattleGameplayAbility_TargetedAttack::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, AreaCenterData);
}

void UBattleGameplayAbility_TargetedAttack::ServerRPCNotifyHit_Implementation(const FHitResult& HitResult,
	float HitCheckTime)
{
	Super::ServerRPCNotifyHit_Implementation(HitResult, HitCheckTime);
}

void UBattleGameplayAbility_TargetedAttack::AttackHitConfirm(const FHitResult& HitResult)
{
	Super::AttackHitConfirm(HitResult);
}

void UBattleGameplayAbility_TargetedAttack::OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& InData,
	FGameplayTag ApplicationTag)
{
	Super::OnTargetDataReadyCallback(InData, ApplicationTag);

	if (GetWorld()->GetNetMode() != NM_Client)
	{
		for (TSubclassOf<UGameplayEffect> TargetGE : CurrentAttackData->AppliedEffectsToTarget)
		{
			FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(TargetGE, 1);

			ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, SpecHandle, InData);		
		}

		for (TSubclassOf<UGameplayEffect> SelfGE : CurrentAttackData->AppliedEffectsToSelf)
		{
			FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(SelfGE, 1);

			ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, SpecHandle);
		}
		
		OnTargetDataReady(InData);
	}
}

void UBattleGameplayAbility_TargetedAttack::SelectHitCheck(const FHitResult HitResult, const float AttackTime)
{
	Super::SelectHitCheck(HitResult, AttackTime);
}

void UBattleGameplayAbility_TargetedAttack::OnCompleted()
{
	Super::OnCompleted();
}

void UBattleGameplayAbility_TargetedAttack::OnInterrupted()
{
	Super::OnInterrupted();
}

void UBattleGameplayAbility_TargetedAttack::OnRep_AlreadyHitActors()
{
	Super::OnRep_AlreadyHitActors();
}


void UBattleGameplayAbility_TargetedAttack::StartHitCheck(FGameplayTag Channel, const FBattleVerbMessage& Notification)
{
	Super::StartHitCheck(Channel, Notification);
	
	AlreadyHitActors.Empty();
	
	if (ABattleCharacterBase* Character = Cast<ABattleCharacterBase>(GetAvatarActorFromActorInfo()))
	{
		UBattleCombatManagerComponent* CurrentCombatManager = CastChecked<UBattleCombatManagerComponent>(Character->GetComponentByClass(UBattleCombatManagerComponent::StaticClass()));

		UAttackCollisionMethod* CollisionMethod = CurrentCombatManager->GetCollisionMethod(CurrentAttackData->CollisionMethod->CollisionMethodType);
		if (UAttackCollisionMethod_CircularAOE* CircularAOE = Cast<UAttackCollisionMethod_CircularAOE>(CollisionMethod))
		{
			CircularAOE->SetAreaCenterData(AreaCenterData);
		}
		CollisionMethod->SetCollisionData(CurrentAttackData->CollisionMethod, this);
		CollisionMethod->StartCollisionCheck();
	}
	
}

void UBattleGameplayAbility_TargetedAttack::EndHitCheck(FGameplayTag Channel, const FBattleVerbMessage& Notification)
{
	if (ABattleCharacterBase* Character = Cast<ABattleCharacterBase>(GetAvatarActorFromActorInfo()))
	{
		UBattleCombatManagerComponent* CurrentCombatManager = CastChecked<UBattleCombatManagerComponent>(Character->GetComponentByClass(UBattleCombatManagerComponent::StaticClass()));

		UAttackCollisionMethod* CollisionMethod = CurrentCombatManager->GetCollisionMethod(CurrentAttackData->CollisionMethod->CollisionMethodType);
		
		CollisionMethod->EndCollisionCheck();
		
	}
}

void UBattleGameplayAbility_TargetedAttack::OnRep_AreaCenterData()
{
}
