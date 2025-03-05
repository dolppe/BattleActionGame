#include "BattleGameplayAbility_ComboStrongAttack.h"

#include "BattleAbilityTask_HitCheck.h"
#include "BattleCombatManagerComponent.h"
#include "GameplayMessageSubsystem.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "BattleActionGame/BattleGameplayTags.h"
#include "BattleActionGame/Character/BattleCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleGameplayAbility_ComboStrongAttack)

UBattleGameplayAbility_ComboStrongAttack::UBattleGameplayAbility_ComboStrongAttack(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AttackType = EAttackType::ComboStrong;
}

void UBattleGameplayAbility_ComboStrongAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	const ABattleCharacterBase* Character = Cast<ABattleCharacterBase>(ActorInfo->AvatarActor);
	UBattleCombatManagerComponent* CurrentCombatManager = CastChecked<UBattleCombatManagerComponent>(Character->GetComponentByClass(UBattleCombatManagerComponent::StaticClass()));

	const FComboStrongAttack& CurrentAttackData = CurrentCombatManager->GetAttackData()->ComboStrongAttacks[AttackMode];
	CurrentAttackMontage = CurrentCombatManager->GetAttackMontage(EAttackType::ComboStrong, AttackMode);

	AttackRate = CurrentAttackData.AttackRate;
	GroggyValue = CurrentAttackData.GroggyValue;

	const FName MontageSectionName = *FString::Printf(TEXT("%s%d"), *CurrentAttackData.MontageSectionName, CurrentCombatManager->GetCurrentComboIndex());

	UAbilityTask_PlayMontageAndWait* PlayAttackMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("PlayMontage"), CurrentAttackMontage, 1.0f, MontageSectionName);
	PlayAttackMontage->OnCompleted.AddDynamic(this, &UBattleGameplayAbility_ComboStrongAttack::OnCompleted);
	PlayAttackMontage->OnInterrupted.AddDynamic(this, &UBattleGameplayAbility_ComboStrongAttack::OnInterrupted);
	PlayAttackMontage->OnBlendOut.AddDynamic(this, &UBattleGameplayAbility_ComboStrongAttack::OnBlendOut);
	PlayAttackMontage->ReadyForActivation();

	if (Character->IsLocallyControlled())
	{

		UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());

		StartListenerHandle = MessageSystem.RegisterListener(FBattleGameplayTags::Get().Combat_Attack_Event_Start, this, &UBattleGameplayAbility_ComboStrongAttack::StartHitCheck);
		EndListenerHandle = MessageSystem.RegisterListener(FBattleGameplayTags::Get().Combat_Attack_Event_End, this, &UBattleGameplayAbility_ComboStrongAttack::EndHitCheck);
	}
	if (GetWorld()->GetNetMode() != NM_Client)
	{
		Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
		AlreadyHitActors.Reset();
	}
	
}

void UBattleGameplayAbility_ComboStrongAttack::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	ABattleCharacterBase* Character = Cast<ABattleCharacterBase>(ActorInfo->AvatarActor);
	
	if (Character->IsLocallyControlled())
	{
		// 테스크 실행
	}
	if (GetWorld()->GetNetMode() != NM_Client)
	{
		// 몽타주 멀티캐스트 필요
		Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UBattleGameplayAbility_ComboStrongAttack::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}


void UBattleGameplayAbility_ComboStrongAttack::ServerRPCNotifyHit_Implementation(const FHitResult& HitResult, float HitCheckTime)
{
	Super::ServerRPCNotifyHit_Implementation(HitResult, HitCheckTime);
}


void UBattleGameplayAbility_ComboStrongAttack::AttackHitConfirm(const FHitResult& HitResult)
{
	Super::AttackHitConfirm(HitResult);
}

void UBattleGameplayAbility_ComboStrongAttack::OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& InData,
                                                              FGameplayTag ApplicationTag)
{
	Super::OnTargetDataReadyCallback(InData, ApplicationTag);
	
	if (GetWorld()->GetNetMode() != NM_Client)
	{
		OnTargetDataReady(InData);
	}
}

void UBattleGameplayAbility_ComboStrongAttack::SelectHitCheck(const FHitResult HitResult, const float AttackTime)
{
	Super::SelectHitCheck(HitResult, AttackTime);
}

void UBattleGameplayAbility_ComboStrongAttack::OnCompleted()
{
	Super::OnCompleted();
	
}

void UBattleGameplayAbility_ComboStrongAttack::OnInterrupted()
{
	Super::OnInterrupted();
}

void UBattleGameplayAbility_ComboStrongAttack::OnRep_AlreadyHitActors()
{
}

void UBattleGameplayAbility_ComboStrongAttack::StartHitCheck(FGameplayTag Channel, const FBattleVerbMessage& Notification)
{

	AlreadyHitActors.Empty();
	
	if (ABattleCharacterBase* Character = Cast<ABattleCharacterBase>(GetAvatarActorFromActorInfo()))
	{
		UBattleCombatManagerComponent* CurrentCombatManager = CastChecked<UBattleCombatManagerComponent>(Character->GetComponentByClass(UBattleCombatManagerComponent::StaticClass()));

		const FComboStrongAttack& CurrentAttackData = CurrentCombatManager->GetAttackData()->ComboStrongAttacks[AttackMode];

		UAttackCollisionMethod* CollisionMethod = CurrentCombatManager->GetCollisionMethod(CurrentAttackData.CollisionMethod->CollisionMethodType);
		CollisionMethod->SetCollisionData(CurrentAttackData.CollisionMethod, this);
		CollisionMethod->StartCollisionCheck();
	}

	/*
	 * 처리 팔요
	 */
	
	// HitCheckTask = UBattleAbilityTask_HitCheck::CreateTask(this);
	// HitCheckTask->SetHitCheckData(CurrentAttackData.StartSocketName, CurrentAttackData.EndSocketName, CurrentAttackData.AttackRadius, CurrentAttackData.CollisionChannel);
	// HitCheckTask->OnHitChecked.AddDynamic(this, &UBattleGameplayAbility_ComboStrongAttack::SelectHitCheck);
	// HitCheckTask->ReadyForActivation();
}

void UBattleGameplayAbility_ComboStrongAttack::EndHitCheck(FGameplayTag Channel, const FBattleVerbMessage& Notification)
{
	if (ABattleCharacterBase* Character = Cast<ABattleCharacterBase>(GetAvatarActorFromActorInfo()))
	{
		UBattleCombatManagerComponent* CurrentCombatManager = CastChecked<UBattleCombatManagerComponent>(Character->GetComponentByClass(UBattleCombatManagerComponent::StaticClass()));
		const FComboStrongAttack& CurrentAttackData = CurrentCombatManager->GetAttackData()->ComboStrongAttacks[AttackMode];
		UAttackCollisionMethod* CollisionMethod = CurrentCombatManager->GetCollisionMethod(CurrentAttackData.CollisionMethod->CollisionMethodType);
		CollisionMethod->EndCollisionCheck();
	}
}
