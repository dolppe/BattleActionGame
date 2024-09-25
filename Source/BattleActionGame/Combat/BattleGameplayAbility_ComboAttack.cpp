#include "BattleGameplayAbility_ComboAttack.h"

#include "BattleAbilityTask_HitCheck.h"
#include "BattleCombatManagerComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_Repeat.h"
#include "BattleActionGame/Character/BattleCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleGameplayAbility_ComboAttack)

UBattleGameplayAbility_ComboAttack::UBattleGameplayAbility_ComboAttack(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
}

void UBattleGameplayAbility_ComboAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	CurrentComboIndex = 0;
	bAllowedInput = false;
	bHasNextComboInput = false;

	ABattleCharacter* Character = Cast<ABattleCharacter>(ActorInfo->AvatarActor);
	UBattleCombatManagerComponent* CombatManagerComponent = CastChecked<UBattleCombatManagerComponent>(Character->GetComponentByClass(UBattleCombatManagerComponent::StaticClass()));

	CurrentComboData = CombatManagerComponent->GetComboData(AttackMode);
	CurrentComboMontage = CombatManagerComponent->GetComboMontage(AttackMode);

	FName MontageSectionName = GetNextSection();

	UAbilityTask_PlayMontageAndWait* PlayAttackMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("PlayMontage"), CurrentComboMontage, 1.0f, MontageSectionName);
	PlayAttackMontage->OnCompleted.AddDynamic(this, &UBattleGameplayAbility_ComboAttack::OnCompleted);
	PlayAttackMontage->OnInterrupted.AddDynamic(this, &UBattleGameplayAbility_ComboAttack::OnInterrupted);
	PlayAttackMontage->ReadyForActivation();


	
	if (Character->IsLocallyControlled())
	{
		UE_LOG(LogTemp, Log, TEXT("ClientAttackStart"));
		StartComboTimer(MontageSectionName);
		UBattleAbilityTask_HitCheck* HitCheck = UBattleAbilityTask_HitCheck::CreateTask(this);
		HitCheck->OnHitChecked.AddDynamic(this, &UBattleGameplayAbility_ComboAttack::SelectHitCheck);
		HitCheck->ReadyForActivation();
	}
	else if (HasAuthority(&ActivationInfo))
	{
		Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
		UE_LOG(LogTemp, Log, TEXT("ServerAttackStart"));
		AlreadyHitActors.Reset();
	}

	
	
}

void UBattleGameplayAbility_ComboAttack::InputPressed(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);

	ABattleCharacter* Character = Cast<ABattleCharacter>(ActorInfo->AvatarActor);

	if (bAllowedInput)
	{
		bHasNextComboInput = true;
		UE_LOG(LogTemp, Log, TEXT("InputPressed"));
	}
}

void UBattleGameplayAbility_ComboAttack::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	ABattleCharacter* Character = Cast<ABattleCharacter>(ActorInfo->AvatarActor);
	
	if (Character->IsLocallyControlled())
	{
		// 테스크 실행
		UE_LOG(LogTemp, Log, TEXT("ClientEndAbility"));
	}
	else
	{
		// 몽타주 멀티캐스트 필요
		UE_LOG(LogTemp, Log, TEXT("ServerEndAbility"));
		Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		AlreadyHitActors.Reset();
	}


	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UBattleGameplayAbility_ComboAttack::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UBattleGameplayAbility_ComboAttack, AlreadyHitActors);
}

FName UBattleGameplayAbility_ComboAttack::GetNextSection()
{
	CurrentComboIndex = FMath::Clamp(CurrentComboIndex+1,1,CurrentComboData->MaxComboCount);
	FName NextSection = *FString::Printf(TEXT("%s%d"), *CurrentComboData->MontageSectionName, CurrentComboIndex);
	return NextSection;
}

void UBattleGameplayAbility_ComboAttack::StartComboTimer(FName MontageSectionName)
{
	ensure(CurrentComboData->AllowInputFrameCount.IsValidIndex(CurrentComboIndex-1));

	const float CurrentMontageSectionLength = CurrentComboMontage->GetSectionLength(CurrentComboMontage->GetSectionIndex(MontageSectionName));

	float ComboInputCheckTime = CurrentMontageSectionLength*0.7;
	const float AllowedInputTimeRate = CurrentComboData->AllowInputFrameCount[CurrentComboIndex-1] / CurrentComboData->FrameRate;
	float AllowedInputTime = CurrentMontageSectionLength * AllowedInputTimeRate;

	UE_LOG(LogTemp, Log, TEXT("MontageLength: %f"), CurrentMontageSectionLength);
	UE_LOG(LogTemp,Log,TEXT("AllowedInputTime: %f"), AllowedInputTime);
	UE_LOG(LogTemp, Log, TEXT("ComboInputCheckTime: %f"), ComboInputCheckTime);

	if (AllowedInputTime >0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(ComboAllowedTimerHandle, this, &UBattleGameplayAbility_ComboAttack::AllowInput, AllowedInputTime, false);
		GetWorld()->GetTimerManager().SetTimer(ComboTimerHandle, this, &UBattleGameplayAbility_ComboAttack::CheckComboInput, ComboInputCheckTime, false);
	}

}

void UBattleGameplayAbility_ComboAttack::CheckComboInput()
{
	UE_LOG(LogTemp, Log, TEXT("ComboCheck"));
	ComboTimerHandle.Invalidate();
	if (bHasNextComboInput)
	{
		UE_LOG(LogTemp, Log, TEXT("ComboCheck => Success"));
		FName MontageSection = GetNextSection();
		MontageJumpToSection(MontageSection);
		ServerRPCMontageSectionChanged();
		StartComboTimer(MontageSection);
		bHasNextComboInput = false;
		bAllowedInput = false;
	}
}

void UBattleGameplayAbility_ComboAttack::AllowInput()
{
	UE_LOG(LogTemp, Log, TEXT("AllowedInput"));
	ComboAllowedTimerHandle.Invalidate();
	bAllowedInput = true;
}

void UBattleGameplayAbility_ComboAttack::ServerRPCMontageSectionChanged_Implementation()
{
	AlreadyHitActors.Reset();
}

void UBattleGameplayAbility_ComboAttack::SelectHitCheck(const FHitResult HitResult, const float AttackTime)
{
	if (!AlreadyHitActors.Contains(HitResult.GetActor()))
	{
		ServerRPCNotifyHit(HitResult, AttackTime);
	}	
}

void UBattleGameplayAbility_ComboAttack::ServerRPCNotifyHit_Implementation(const FHitResult& HitResult,
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
				AlreadyHitActors.Add(HitActor);
				UE_LOG(LogTemp, Log, TEXT("Hit Success => Damage"));
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("Hit Rejected"));
			}
		}
	}

}

bool UBattleGameplayAbility_ComboAttack::ServerRPCNotifyHit_Validate(const FHitResult& HitResult, float HitCheckTime)
{
	// HitCheckTime을 통해서 Validate
	return true;
}

void UBattleGameplayAbility_ComboAttack::OnCompleted()
{
	ABattleCharacter* Character = Cast<ABattleCharacter>(GetActorInfo().AvatarActor);
		
	UE_LOG(LogTemp, Log, TEXT("MontageEnd"));
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}

void UBattleGameplayAbility_ComboAttack::OnInterrupted()
{

	bool bReplicatedEndAbility = true;
	bool bWasCancelled = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}

void UBattleGameplayAbility_ComboAttack::OnRep_AlreadyHitActors()
{

}
