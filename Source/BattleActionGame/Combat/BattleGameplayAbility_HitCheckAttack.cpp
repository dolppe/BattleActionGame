#include "BattleGameplayAbility_HitCheckAttack.h"

#include "BattleCombatManagerComponent.h"
#include "GameplayMessageSubsystem.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "BattleActionGame/BattleGameplayTags.h"
#include "BattleActionGame/Character/BattleCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BattleActionGame/Messages/BattleVerbMessage.h"
#include "BattleActionGame/Physics/BattleCollisionChannels.h"
#include "GameFramework/GameStateBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleGameplayAbility_HitCheckAttack)

UBattleGameplayAbility_HitCheckAttack::UBattleGameplayAbility_HitCheckAttack(
	const FObjectInitializer& ObjectInitializer)
		: Super(ObjectInitializer)
{
}

void UBattleGameplayAbility_HitCheckAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	const ABattleCharacterBase* Character = Cast<ABattleCharacterBase>(ActorInfo->AvatarActor);
	UBattleCombatManagerComponent* CurrentCombatManager = CastChecked<UBattleCombatManagerComponent>(Character->GetComponentByClass(UBattleCombatManagerComponent::StaticClass()));

	const FHitCheckAttack& CurrentHitCheckData = CurrentCombatManager->GetHitCheckAttackData(AttackMode);
	CurrentAttackMontage = CurrentCombatManager->GetAttackMontage(EAttackType::HitCheck, AttackMode);

	AttackRate = CurrentHitCheckData.AttackRate;

	const FName MontageSectionName = *FString::Printf(TEXT("%s%d"), *CurrentHitCheckData.MontageSectionName, 1);

	UAbilityTask_PlayMontageAndWait* PlayAttackMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("PlayMontage"), CurrentAttackMontage, 0.65f, MontageSectionName);
	PlayAttackMontage->OnCompleted.AddDynamic(this, &UBattleGameplayAbility_HitCheckAttack::OnCompleted);
	PlayAttackMontage->OnInterrupted.AddDynamic(this, &UBattleGameplayAbility_HitCheckAttack::OnInterrupted);
	PlayAttackMontage->OnBlendOut.AddDynamic(this, &UBattleGameplayAbility_HitCheckAttack::OnBlendOut);
	PlayAttackMontage->ReadyForActivation();

	if (Character->IsLocallyControlled())
	{
		UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());

		StartListenerHandle = MessageSystem.RegisterListener(FBattleGameplayTags::Get().Combat_Attack_Event_Start, this, &UBattleGameplayAbility_HitCheckAttack::StartHitCheck);
		
	}
	if (GetWorld()->GetNetMode() != NM_Client)
	{
		Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
		UE_LOG(LogTemp, Log, TEXT("ServerAttackStart"));
		AlreadyHitActors.Reset();
	}
	
	
}

void UBattleGameplayAbility_HitCheckAttack::EndAbility(const FGameplayAbilitySpecHandle Handle,
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
		UE_LOG(LogTemp, Log, TEXT("ServerEndAbility"));
		Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UBattleGameplayAbility_HitCheckAttack::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UBattleGameplayAbility_HitCheckAttack::ServerRPCNotifyHit_Implementation(const FHitResult& HitResult,
	float HitCheckTime)
{
	Super::ServerRPCNotifyHit_Implementation(HitResult, HitCheckTime);
}

void UBattleGameplayAbility_HitCheckAttack::AttackHitConfirm(const FHitResult& HitResult)
{
	Super::AttackHitConfirm(HitResult);
}

void UBattleGameplayAbility_HitCheckAttack::OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& InData,
	FGameplayTag ApplicationTag)
{
	Super::OnTargetDataReadyCallback(InData, ApplicationTag);
	
	if (GetWorld()->GetNetMode() != NM_Client)
	{
		OnTargetDataReady(InData);
	}
}

void UBattleGameplayAbility_HitCheckAttack::SelectHitCheck(const FHitResult HitResult, const float AttackTime)
{
	Super::SelectHitCheck(HitResult, AttackTime);
}

void UBattleGameplayAbility_HitCheckAttack::OnCompleted()
{
	Super::OnCompleted();
}

void UBattleGameplayAbility_HitCheckAttack::OnInterrupted()
{
	Super::OnInterrupted();
}

void UBattleGameplayAbility_HitCheckAttack::OnRep_AlreadyHitActors()
{
}

void UBattleGameplayAbility_HitCheckAttack::AttackEvent(FGameplayTag Channel, const FBattleVerbMessage& Notification)
{

}

void UBattleGameplayAbility_HitCheckAttack::StartHitCheck(FGameplayTag Channel, const FBattleVerbMessage& Notification)
{
		if (Notification.Target == GetAvatarActorFromActorInfo())
	{
		// Trace해서 처리 후 SelectHitCheck에 전송
		TArray<FHitResult> HitResults;

		ABattleCharacterBase* CharacterBase = Cast<ABattleCharacterBase>(GetAvatarActorFromActorInfo());
		USkeletalMeshComponent* SkeletalMesh = CharacterBase->GetMesh();
		
		const FRotator Rotation = CharacterBase->GetActorRotation();
		const FRotator YawRotation(0,Rotation.Yaw,0);

		FCollisionQueryParams Temp;
		
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		
		UBattleCombatManagerComponent* CurrentCombatManager = CastChecked<UBattleCombatManagerComponent>(CharacterBase->GetComponentByClass(UBattleCombatManagerComponent::StaticClass()));
		const FHitCheckAttack& CurrentHitCheckData = CurrentCombatManager->GetHitCheckAttackData(AttackMode);

		FVector Start = SkeletalMesh->GetSocketLocation(*CurrentHitCheckData.StartSocketName);
		FVector End = Start + ForwardDirection*CurrentHitCheckData.AttackRange;

		float Radius = CurrentHitCheckData.AttackRadius;
		
		GetWorld()->SweepMultiByChannel(HitResults, Start, End, FQuat::Identity, CurrentHitCheckData.CollisionChannel, FCollisionShape::MakeSphere(Radius),Temp);
		
		FColor HitColor = FColor::Red; 
	
		for (FHitResult HitResult : HitResults)
		{
			UE_LOG(LogTemp, Log, TEXT("Hit %s"),*HitResult.Component->GetName());
			HitColor = FColor::Green;
			if (HitResult.GetActor()->IsA(ABattleCharacterBase::StaticClass()))
			{
				UE_LOG(LogTemp, Log, TEXT("Enemy Hit %s"), *HitResult.GetActor()->GetName());
				SelectHitCheck(HitResult, GetWorld()->GetGameState()->GetServerWorldTimeSeconds());
			}
		}

#if 1
	
		FVector TraceVector = End - Start;
		FVector CapsuleCenter = (Start + End) * 0.5f;  // 캡슐의 중심은 Start와 End의 중간 지점
		float CapsuleHalfHeight = TraceVector.Size() * 0.5f;  // 캡슐의 절반 길이
		FQuat CapsuleRotation = FQuat::FindBetweenNormals(FVector::UpVector, TraceVector.GetSafeNormal());
		//	DrawDebugLine(GetWorld(), Start, End, HitColor, false, 10.f, 0, 0);
		DrawDebugCapsule(GetWorld(), CapsuleCenter, CapsuleHalfHeight, Radius,CapsuleRotation, HitColor,false, 10.f);
#endif
		
		
	}
	
}
