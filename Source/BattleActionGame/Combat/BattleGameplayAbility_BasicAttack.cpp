#include "BattleGameplayAbility_BasicAttack.h"

#include "BattleCombatManagerComponent.h"
#include "GameplayMessageSubsystem.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "BattleActionGame/BattleGameplayTags.h"
#include "BattleActionGame/Character/BattleCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BattleActionGame/Messages/BattleVerbMessage.h"
#include "BattleActionGame/Physics/BattleCollisionChannels.h"
#include "GameFramework/GameStateBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleGameplayAbility_BasicAttack)

UBattleGameplayAbility_BasicAttack::UBattleGameplayAbility_BasicAttack(
	const FObjectInitializer& ObjectInitializer)
		: Super(ObjectInitializer)
{
	AttackType = EAttackType::Basic;
}

void UBattleGameplayAbility_BasicAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	const ABattleCharacterBase* Character = Cast<ABattleCharacterBase>(ActorInfo->AvatarActor);
	UBattleCombatManagerComponent* CurrentCombatManager = CastChecked<UBattleCombatManagerComponent>(Character->GetComponentByClass(UBattleCombatManagerComponent::StaticClass()));

	AttackData = &CurrentCombatManager->GetAttackData()->BasicAttacks[AttackMode];

	const FName MontageSectionName = *FString::Printf(TEXT("%s"), *AttackData->MontageSectionName);

	UAbilityTask_PlayMontageAndWait* PlayAttackMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("PlayMontage"), AttackData->Montage, 0.65f, MontageSectionName);
	PlayAttackMontage->OnCompleted.AddDynamic(this, &UBattleGameplayAbility_BasicAttack::OnCompleted);
	PlayAttackMontage->OnInterrupted.AddDynamic(this, &UBattleGameplayAbility_BasicAttack::OnInterrupted);
	PlayAttackMontage->OnBlendOut.AddDynamic(this, &UBattleGameplayAbility_BasicAttack::OnBlendOut);
	PlayAttackMontage->ReadyForActivation();

	if (Character->IsLocallyControlled())
	{

	}
	if (GetWorld()->GetNetMode() != NM_Client)
	{

	}
	
	
}

void UBattleGameplayAbility_BasicAttack::EndAbility(const FGameplayAbilitySpecHandle Handle,
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
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UBattleGameplayAbility_BasicAttack::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UBattleGameplayAbility_BasicAttack::OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& InData,
	FGameplayTag ApplicationTag)
{
	Super::OnTargetDataReadyCallback(InData, ApplicationTag);
	
	OnTargetDataReady(InData);
}


void UBattleGameplayAbility_BasicAttack::OnCompleted()
{
	Super::OnCompleted();
}

void UBattleGameplayAbility_BasicAttack::OnInterrupted()
{
	Super::OnInterrupted();
}


void UBattleGameplayAbility_BasicAttack::AttackEvent(FGameplayTag Channel, const FBattleVerbMessage& Notification)
{

}


