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

	const FBasicAttack& CurrentHitCheckData = CurrentCombatManager->GetAttackData()->BasicAttacks[AttackMode];
	CurrentAttackMontage = CurrentCombatManager->GetAttackMontage(EAttackType::Basic, AttackMode);

	AttackRate = CurrentHitCheckData.AttackRate;
	GroggyValue = CurrentHitCheckData.GroggyValue;

	const FName MontageSectionName = *FString::Printf(TEXT("%s%d"), *CurrentHitCheckData.MontageSectionName, 1);

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

void UBattleGameplayAbility_BasicAttack::ServerRPCNotifyHit_Implementation(const FHitResult& HitResult,
	float HitCheckTime)
{
	Super::ServerRPCNotifyHit_Implementation(HitResult, HitCheckTime);
}

void UBattleGameplayAbility_BasicAttack::AttackHitConfirm(const FHitResult& HitResult)
{
	Super::AttackHitConfirm(HitResult);
}

void UBattleGameplayAbility_BasicAttack::OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& InData,
	FGameplayTag ApplicationTag)
{
	Super::OnTargetDataReadyCallback(InData, ApplicationTag);
	
	if (GetWorld()->GetNetMode() != NM_Client)
	{
		OnTargetDataReady(InData);
	}
}

void UBattleGameplayAbility_BasicAttack::SelectHitCheck(const FHitResult HitResult, const float AttackTime)
{
	Super::SelectHitCheck(HitResult, AttackTime);
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

TArray<FHitResult> UBattleGameplayAbility_BasicAttack::StartHitCheckByWeaponRange()
{
	TArray<FHitResult> HitResults;

	ABattleCharacterBase* CharacterBase = Cast<ABattleCharacterBase>(GetAvatarActorFromActorInfo());
	USkeletalMeshComponent* SkeletalMesh = CharacterBase->GetMesh();

	const FRotator Rotation = CharacterBase->GetActorRotation();
	const FRotator YawRotation(0,Rotation.Yaw,0);

	FCollisionQueryParams Temp;

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		
	UBattleCombatManagerComponent* CurrentCombatManager = CastChecked<UBattleCombatManagerComponent>(CharacterBase->GetComponentByClass(UBattleCombatManagerComponent::StaticClass()));
	const FBasicAttack& CurrentHitCheckData = CurrentCombatManager->GetAttackData()->BasicAttacks[AttackMode];

	/*
	 * 처리 팔요
	 */
	
	// FVector Start = SkeletalMesh->GetSocketLocation(*CurrentHitCheckData.StartSocketName);
	// FVector End = Start + ForwardDirection*CurrentHitCheckData.AttackRange;
	//
	// float Radius = CurrentHitCheckData.AttackRadius;
	// 	
	// GetWorld()->SweepMultiByChannel(HitResults, Start, End, FQuat::Identity, CurrentHitCheckData.CollisionChannel, FCollisionShape::MakeSphere(Radius),Temp);

	FColor HitColor = FColor::Red;

	for (FHitResult HitResult : HitResults)
	{
		if (HitResult.GetActor()->IsA(ABattleCharacterBase::StaticClass()))
		{
			HitColor = FColor::Green;
		}
	}
	
#if 0
	
	FVector TraceVector = End - Start;
	FVector CapsuleCenter = (Start + End) * 0.5f;  // 캡슐의 중심은 Start와 End의 중간 지점
	float CapsuleHalfHeight = TraceVector.Size() * 0.5f;  // 캡슐의 절반 길이
	FQuat CapsuleRotation = FQuat::FindBetweenNormals(FVector::UpVector, TraceVector.GetSafeNormal());
	//	DrawDebugLine(GetWorld(), Start, End, HitColor, false, 10.f, 0, 0);
	DrawDebugCapsule(GetWorld(), CapsuleCenter, CapsuleHalfHeight, Radius,CapsuleRotation, HitColor,false, 10.f);
#endif
	
	return HitResults;
}

TArray<FHitResult> UBattleGameplayAbility_BasicAttack::StartHitCheckByAreaRange()
{
	TArray<FHitResult> OutHitResults;

	if (AttackAreaData.IsEmpty())
	{
		return TArray<FHitResult>();
	}

	ABattleCharacterBase* CharacterBase = Cast<ABattleCharacterBase>(GetAvatarActorFromActorInfo());
	UBattleCombatManagerComponent* CurrentCombatManager = CastChecked<UBattleCombatManagerComponent>(CharacterBase->GetComponentByClass(UBattleCombatManagerComponent::StaticClass()));
	const FBasicAttack& CurrentHitCheckData = CurrentCombatManager->GetAttackData()->BasicAttacks[AttackMode];

	FVector OffSet = FVector(0.1f,0.1f,0.1f);
	
	for (FAttackAreaData AttackAreaDataItem : AttackAreaData)
	{
		TArray<FHitResult> HitResults;

		/*
	 * 처리 팔요
	 */
	
		//GetWorld()->SweepMultiByChannel(HitResults, AttackAreaDataItem.CenterLocation, AttackAreaDataItem.CenterLocation + OffSet,FQuat::Identity, CurrentHitCheckData.CollisionChannel, FCollisionShape::MakeSphere(AttackAreaDataItem.Radius));

		for (FHitResult HitResult : HitResults)
		{
			auto Pred = [&HitResult](const FHitResult& Other)
			{
				return Other.HitObjectHandle == HitResult.HitObjectHandle;
			};

			if (!OutHitResults.ContainsByPredicate(Pred))
			{
				OutHitResults.Add(HitResult);
			}
		}
	}

	return OutHitResults;

	
}

void UBattleGameplayAbility_BasicAttack::StartHitCheck(FGameplayTag Channel, const FBattleVerbMessage& Notification)
{

	if (ABattleCharacterBase* Character = Cast<ABattleCharacterBase>(GetAvatarActorFromActorInfo()))
	{
		UBattleCombatManagerComponent* CurrentCombatManager = CastChecked<UBattleCombatManagerComponent>(Character->GetComponentByClass(UBattleCombatManagerComponent::StaticClass()));

		const FBasicAttack& CurrentBasicData = CurrentCombatManager->GetAttackData()->BasicAttacks[AttackMode];

		UAttackCollisionMethod* CollisionMethod = CurrentCombatManager->GetCollisionMethod(CurrentBasicData.CollisionMethod->CollisionMethodType);
		CollisionMethod->SetCollisionData(CurrentBasicData.CollisionMethod, this);
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

		const FBasicAttack& CurrentBasicData = CurrentCombatManager->GetAttackData()->BasicAttacks[AttackMode];

		UAttackCollisionMethod* CollisionMethod = CurrentCombatManager->GetCollisionMethod(CurrentBasicData.CollisionMethod->CollisionMethodType);
		
		CollisionMethod->EndCollisionCheck();
		
	}
}
