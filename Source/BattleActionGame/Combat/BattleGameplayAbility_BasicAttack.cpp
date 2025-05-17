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

	CurrentBasicAttackData = &CurrentCombatManager->GetAttackData()->BasicAttacks[AttackMode];
	CurrentAttackMontage = CurrentBasicAttackData->Montage;

	BaseDamage = CurrentBasicAttackData->BaseDamage;
	AttackRate = CurrentBasicAttackData->AttackRate;
	GroggyValue = CurrentBasicAttackData->GroggyValue;

	const FName MontageSectionName = *FString::Printf(TEXT("%s%d"), *CurrentBasicAttackData->MontageSectionName, 1);

	UAbilityTask_PlayMontageAndWait* PlayAttackMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("PlayMontage"), CurrentAttackMontage, 0.65f, MontageSectionName);
	PlayAttackMontage->OnCompleted.AddDynamic(this, &UBattleGameplayAbility_BasicAttack::OnCompleted);
	PlayAttackMontage->OnInterrupted.AddDynamic(this, &UBattleGameplayAbility_BasicAttack::OnInterrupted);
	PlayAttackMontage->OnBlendOut.AddDynamic(this, &UBattleGameplayAbility_BasicAttack::OnBlendOut);
	PlayAttackMontage->ReadyForActivation();

	if (Character->IsLocallyControlled())
	{
		UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());

		StartListenerHandle = MessageSystem.RegisterListener(FBattleGameplayTags::Get().Combat_Attack_Event_Start, this, &UBattleGameplayAbility_BasicAttack::StartHitCheck);
	}
	if (GetWorld()->GetNetMode() != NM_Client)
	{
		Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
		AlreadyHitActors.Reset();
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
		Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
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
	
	if (GetWorld()->GetNetMode() != NM_Client)
	{
		for (TSubclassOf<UGameplayEffect> TargetGE : CurrentBasicAttackData->AppliedEffectsToTarget)
		{
			FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(TargetGE, 1);

			ApplyGameplayEffectSpecToTarget(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, SpecHandle, InData);		
		}

		for (TSubclassOf<UGameplayEffect> SelfGE : CurrentBasicAttackData->AppliedEffectsToSelf)
		{
			FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(SelfGE, 1);

			ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, SpecHandle);
		}
		
		OnTargetDataReady(InData);
	}
}


void UBattleGameplayAbility_BasicAttack::OnCompleted()
{
	Super::OnCompleted();
}

void UBattleGameplayAbility_BasicAttack::OnInterrupted()
{
	Super::OnInterrupted();
}

void UBattleGameplayAbility_BasicAttack::OnRep_AlreadyHitActors()
{
}

void UBattleGameplayAbility_BasicAttack::AttackEvent(FGameplayTag Channel, const FBattleVerbMessage& Notification)
{

}

void UBattleGameplayAbility_BasicAttack::StartHitCheck(FGameplayTag Channel, const FBattleVerbMessage& Notification)
{

	AlreadyHitActors.Empty();
	
	if (ABattleCharacterBase* Character = Cast<ABattleCharacterBase>(GetAvatarActorFromActorInfo()))
	{
		UBattleCombatManagerComponent* CurrentCombatManager = CastChecked<UBattleCombatManagerComponent>(Character->GetComponentByClass(UBattleCombatManagerComponent::StaticClass()));

		UAttackCollisionMethod* CollisionMethod = CurrentCombatManager->GetCollisionMethod(CurrentBasicAttackData->CollisionMethod->CollisionMethodType);
		CollisionMethod->SetCollisionData(CurrentBasicAttackData->CollisionMethod, this);
		CollisionMethod->StartCollisionCheck();
	}
	
	// if (Notification.Target == GetAvatarActorFromActorInfo())
	// {
	// 	// Trace해서 처리 후 SelectHitCheck에 전송
	// 	TArray<FHitResult> HitResults;
	// 	
	// 	if (HitCheckAttackType == EHitCheckAttackType::WeaponRange)
	// 	{
	// 		HitResults = StartHitCheckByWeaponRange();
	// 	}
	// 	else if (HitCheckAttackType == EHitCheckAttackType::AreaRange)
	// 	{
	// 		HitResults = StartHitCheckByAreaRange();
	// 	}
	//
	// 	for (FHitResult HitResult : HitResults)
	// 	{
	// 		if (HitResult.GetActor()->IsA(ABattleCharacterBase::StaticClass()))
	// 		{
	// 			SelectHitCheck(HitResult, GetWorld()->GetGameState()->GetServerWorldTimeSeconds());
	// 		}
	// 	}
	// }
	
}

void UBattleGameplayAbility_BasicAttack::EndHitCheck(FGameplayTag Channel, const FBattleVerbMessage& Notification)
{
	if (ABattleCharacterBase* Character = Cast<ABattleCharacterBase>(GetAvatarActorFromActorInfo()))
	{
		UBattleCombatManagerComponent* CurrentCombatManager = CastChecked<UBattleCombatManagerComponent>(Character->GetComponentByClass(UBattleCombatManagerComponent::StaticClass()));

		UAttackCollisionMethod* CollisionMethod = CurrentCombatManager->GetCollisionMethod(CurrentBasicAttackData->CollisionMethod->CollisionMethodType);
		
		CollisionMethod->EndCollisionCheck();
		
	}
}
