#include "BattleGameplayAbility_ComboStrongAttack.h"

#include "BattleAbilityTask_HitCheck.h"
#include "BattleCombatManagerComponent.h"
#include "GameplayMessageSubsystem.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "BattleActionGame/BattleGameplayTags.h"
#include "BattleActionGame/BattleLogChannels.h"
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

	AttackData = &CurrentCombatManager->GetAttackData()->ComboStrongAttacks[AttackMode];

	const FName MontageSectionName = *FString::Printf(TEXT("%s%d"), *AttackData->MontageSectionName, CurrentCombatManager->GetCurrentComboIndex());

	BA_DEFAULT_LOG(LogBattle, Log, TEXT("%s"), *MontageSectionName.ToString());

	UAbilityTask_PlayMontageAndWait* PlayAttackMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("PlayMontage"), AttackData->Montage, 1.0f, MontageSectionName);
	PlayAttackMontage->OnCompleted.AddDynamic(this, &UBattleGameplayAbility_ComboStrongAttack::OnCompleted);
	PlayAttackMontage->OnInterrupted.AddDynamic(this, &UBattleGameplayAbility_ComboStrongAttack::OnInterrupted);
	PlayAttackMontage->OnBlendOut.AddDynamic(this, &UBattleGameplayAbility_ComboStrongAttack::OnBlendOut);
	PlayAttackMontage->ReadyForActivation();

	if (Character->IsLocallyControlled())
	{

		UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());

	}
	if (GetWorld()->GetNetMode() != NM_Client)
	{

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
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UBattleGameplayAbility_ComboStrongAttack::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UBattleGameplayAbility_ComboStrongAttack::OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& InData,
                                                              FGameplayTag ApplicationTag)
{
	Super::OnTargetDataReadyCallback(InData, ApplicationTag);
	
	OnTargetDataReady(InData);
	
}

void UBattleGameplayAbility_ComboStrongAttack::OnCompleted()
{
	Super::OnCompleted();
	
}

void UBattleGameplayAbility_ComboStrongAttack::OnInterrupted()
{
	Super::OnInterrupted();
}

