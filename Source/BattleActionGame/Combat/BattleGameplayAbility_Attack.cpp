#include "BattleGameplayAbility_Attack.h"

#include "BattleAbilityTask_HitCheck.h"
#include "BattleCombatManagerComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "BattleActionGame/Character/BattleCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleGameplayAbility_Attack)

UBattleGameplayAbility_Attack::UBattleGameplayAbility_Attack(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UBattleGameplayAbility_Attack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ABattleCharacter* Character = Cast<ABattleCharacter>(ActorInfo->AvatarActor);
	UBattleCombatManagerComponent* CurrentCombatManager = CastChecked<UBattleCombatManagerComponent>(Character->GetComponentByClass(UBattleCombatManagerComponent::StaticClass()));

	CurrentAttackData = CurrentCombatManager->GetAttackData(AttackMode);
	CurrentAttackMontage = CurrentCombatManager->GetAttackMontage(AttackMode);

	const FName MontageSectionName = *FString::Printf(TEXT("%s%d"), *CurrentAttackData->MontageSectionName, CurrentCombatManager->GetCurrentComboIndex());

	UAbilityTask_PlayMontageAndWait* PlayAttackMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("PlayMontage"), CurrentAttackMontage, 1.0f, MontageSectionName);
	PlayAttackMontage->OnCompleted.AddDynamic(this, &UBattleGameplayAbility_Attack::OnCompleted);
	PlayAttackMontage->OnInterrupted.AddDynamic(this, &UBattleGameplayAbility_Attack::OnInterrupted);
	PlayAttackMontage->ReadyForActivation();

	if (Character->IsLocallyControlled())
	{
		UE_LOG(LogTemp, Log, TEXT("ClientAttackStart"));
		UBattleAbilityTask_HitCheck* HitCheck = UBattleAbilityTask_HitCheck::CreateTask(this);
		HitCheck->OnHitChecked.AddDynamic(this, &UBattleGameplayAbility_Attack::SelectHitCheck);
		HitCheck->ReadyForActivation();
	}
	else if (GetWorld()->GetNetMode() != NM_Client)
	{
		Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
		UE_LOG(LogTemp, Log, TEXT("ServerAttackStart"));
		AlreadyHitActors.Reset();
	}
	
}

void UBattleGameplayAbility_Attack::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	ABattleCharacter* Character = Cast<ABattleCharacter>(ActorInfo->AvatarActor);
	
	if (GetWorld()->GetNetMode() == NM_Client)
	{
		// 테스크 실행
		UE_LOG(LogTemp, Log, TEXT("ClientEndAbility"));
	}
	else
	{
		// 몽타주 멀티캐스트 필요
		UE_LOG(LogTemp, Log, TEXT("ServerEndAbility"));
		Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UBattleGameplayAbility_Attack::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}


void UBattleGameplayAbility_Attack::ServerRPCNotifyHit_Implementation(const FHitResult& HitResult, float HitCheckTime)
{
	Super::ServerRPCNotifyHit_Implementation(HitResult, HitCheckTime);
}


void UBattleGameplayAbility_Attack::AttackHitConfirm(const FHitResult& HitResult)
{
	Super::AttackHitConfirm(HitResult);
}

void UBattleGameplayAbility_Attack::OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& InData,
                                                              FGameplayTag ApplicationTag)
{
	if (GetWorld()->GetNetMode() != NM_Client)
	{
		OnTargetDataReady(InData);
	}
}

void UBattleGameplayAbility_Attack::SelectHitCheck(const FHitResult HitResult, const float AttackTime)
{
	Super::SelectHitCheck(HitResult, AttackTime);
}

void UBattleGameplayAbility_Attack::OnCompleted()
{
	Super::OnCompleted();
	
}

void UBattleGameplayAbility_Attack::OnInterrupted()
{
	Super::OnInterrupted();
}

void UBattleGameplayAbility_Attack::OnRep_AlreadyHitActors()
{
}
