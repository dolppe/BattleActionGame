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
		: Super(ObjectInitializer)
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

	const FName MontageSectionName = *FString::Printf(TEXT("%s"), *AttackData->MontageSectionName);

	UAbilityTask_PlayMontageAndWait* PlayAttackMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("PlayMontage"), AttackData->Montage, 0.65f, MontageSectionName);
	PlayAttackMontage->OnCompleted.AddDynamic(this, &UBattleGameplayAbility_JustGuardAttack::OnCompleted);
	PlayAttackMontage->OnInterrupted.AddDynamic(this, &UBattleGameplayAbility_JustGuardAttack::OnInterrupted);
	PlayAttackMontage->OnBlendOut.AddDynamic(this, &UBattleGameplayAbility_JustGuardAttack::OnBlendOut);
	PlayAttackMontage->ReadyForActivation();
	
	bJustGuardSucceeded = false;
	PendingHitMessages.Empty();
}

void UBattleGameplayAbility_JustGuardAttack::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                                        const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                        bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UBattleGameplayAbility_JustGuardAttack::ReceivedHits(const FBattleHitMessage& HitMessage)
{
	// 방어 성공 후에는 Hit 무시
	if (bJustGuardSucceeded)
	{
		return;
	}
	
	// 방어 입력 허용 기간에는 판정이 끝날 때까지 Hit 보류
	if (bAllowGuardEvent)
	{
		PendingHitMessages.Add(HitMessage);
	}
	else
	{
		// 입력 허용 기간이 지났으면 즉시 피격 처리
		Super::ReceivedHits(HitMessage);
	}
}

void UBattleGameplayAbility_JustGuardAttack::AllowGuardEvent()
{
	// 방어 입력 기회는 서버에서만 부여
	if (GetWorld()->GetNetMode() == NM_Client)
	{
		return;
	}
	
	bAllowGuardEvent = true;
	
	// 공격자 주변 플레이어 탐색
	AActor* AttackActor = GetAvatarActorFromActorInfo();
	TArray<FOverlapResult> OverlapResults;
	
	GetWorld()->OverlapMultiByChannel(OverlapResults, AttackActor->GetActorLocation(), 
		FQuat::Identity, Battle_TraceChannel_AttackToCharacter, FCollisionShape::MakeSphere(AllowGuardRadius));

	for (const FOverlapResult& OverlapResult : OverlapResults)
	{
		if (ABattleCharacter* PlayerCharacter = Cast<ABattleCharacter>(OverlapResult.GetActor()))
		{
			// 공격자와 입력 허용 시간을 전달하며 일회성 방어 능력 활성화
			FGameplayAbilitySpec TryGuardAbilitySpec(UBattleGameplayAbility_TryJustGuard::StaticClass());
			FGameplayEventData TryGuardEventData;

			TryGuardEventData.Instigator = GetAvatarActorFromActorInfo();
			TryGuardEventData.EventMagnitude = AllowGuardEventTime;
			
			PlayerCharacter->GetAbilitySystemComponent()->GiveAbilityAndActivateOnce(TryGuardAbilitySpec, &TryGuardEventData);
		}
	}
	// 허용 시간이 지나면 입력 기회를 종료하고 보류한 Hit 처리
	GetWorld()->GetTimerManager().SetTimer(AllowGuardEventHandle, this, &ThisClass::DisallowGuardEvent, AllowGuardEventTime, false);
}

void UBattleGameplayAbility_JustGuardAttack::DisallowGuardEvent()
{
	bAllowGuardEvent = false;

	// 방어 성공 시 보류한 Hit 결과 폐기
	if (bJustGuardSucceeded)
	{
		PendingHitMessages.Reset();
		return;
	}
	
	// 방어 실패 시 보류한 Hit를 일반 피격 흐름으로 처리
	for (const FBattleHitMessage& HitMessage : PendingHitMessages)
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
	
	PendingHitMessages.Reset();
}

void UBattleGameplayAbility_JustGuardAttack::ReceivedGuard(AActor* TryActor)
{
	if (bAllowGuardEvent)
	{
		bJustGuardSucceeded = true;
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
