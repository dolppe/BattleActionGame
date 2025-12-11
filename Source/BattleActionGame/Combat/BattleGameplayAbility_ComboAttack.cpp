#include "BattleGameplayAbility_ComboAttack.h"

#include "AbilitySystemComponent.h"
#include "BattleAbilityTask_HitCheck.h"
#include "BattleCombatManagerComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "BattleActionGame/BattleGameplayTags.h"
#include "BattleActionGame/BattleLogChannels.h"
#include "BattleActionGame/Character/BattleCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleGameplayAbility_ComboAttack)

UBattleGameplayAbility_ComboAttack::UBattleGameplayAbility_ComboAttack(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AttackType = EAttackType::Combo;
}

void UBattleGameplayAbility_ComboAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	CurrentComboIndex = 0;
	bAllowedInput = false;
	bHasNextComboInput = false;

	const ABattleCharacterBase* Character = Cast<ABattleCharacterBase>(ActorInfo->AvatarActor);
	CurrentCombatManager = CastChecked<UBattleCombatManagerComponent>(Character->GetComponentByClass(UBattleCombatManagerComponent::StaticClass()));

	CurrentComboAttackData = &CurrentCombatManager->GetAttackData()->ComboAttacks[AttackMode];
	AttackData = CurrentComboAttackData;

	FName MontageSectionName = GetNextSection();

	UAbilityTask_PlayMontageAndWait* PlayAttackMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("PlayMontage"), AttackData->Montage, 1.0f, MontageSectionName);
	PlayAttackMontage->OnCompleted.AddDynamic(this, &UBattleGameplayAbility_ComboAttack::OnCompleted);
	PlayAttackMontage->OnInterrupted.AddDynamic(this, &UBattleGameplayAbility_ComboAttack::OnInterrupted);
	PlayAttackMontage->OnBlendOut.AddDynamic(this, &UBattleGameplayAbility_ComboAttack::OnBlendOut);
	PlayAttackMontage->ReadyForActivation();

	CurrentCombatManager->SetComboGA(this);
	
	if (Character->IsLocallyControlled())
	{
		StartComboTimer(MontageSectionName);
		
	}
	if (GetWorld()->GetNetMode() != NM_Client)
	{

	}
}

void UBattleGameplayAbility_ComboAttack::InputPressed(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
		
	if (CurrentComboAttackData->MaxComboCount <= CurrentComboIndex)
	{
		return;
	}

	if (bAllowedInput && !bHasNextComboInput)
	{
		bHasNextComboInput = true;
	}
}

void UBattleGameplayAbility_ComboAttack::EndAbility(const FGameplayAbilitySpecHandle Handle,
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
	
	CurrentCombatManager->OnEndAbilityComboGA();
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UBattleGameplayAbility_ComboAttack::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UBattleGameplayAbility_ComboAttack, bHasNextComboInput);
}

FName UBattleGameplayAbility_ComboAttack::GetNextSection()
{
	CurrentComboIndex = FMath::Clamp(CurrentComboIndex+1,1,CurrentComboAttackData->MaxComboCount);
	FName NextSection = *FString::Printf(TEXT("%s%d"), *CurrentComboAttackData->MontageSectionName, CurrentComboIndex);
	return NextSection;
}

void UBattleGameplayAbility_ComboAttack::StartComboTimer(FName MontageSectionName)
{

	const float CurrentMontageSectionLength = AttackData->Montage->GetSectionLength(AttackData->Montage->GetSectionIndex(MontageSectionName));

	float ComboInputCheckTime = CurrentMontageSectionLength*0.7;
	const float AllowedInputTimeRate = CurrentComboAttackData->AllowInputFrameCount[CurrentComboIndex-1] / CurrentComboAttackData->FrameRate;
	float AllowedInputTime = CurrentMontageSectionLength * AllowedInputTimeRate;
	

	if (AllowedInputTime >0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(ComboAllowedTimerHandle, this, &UBattleGameplayAbility_ComboAttack::AllowInput, AllowedInputTime, false);
		GetWorld()->GetTimerManager().SetTimer(ComboTimerHandle, this, &UBattleGameplayAbility_ComboAttack::CheckComboInput, ComboInputCheckTime, false);
	}

}

void UBattleGameplayAbility_ComboAttack::CheckComboInput()
{
	ComboTimerHandle.Invalidate();
	
	if (bHasNextComboInput)
	{
		FName MontageSection = GetNextSection();
		MontageJumpToSection(MontageSection);
		ServerRPCMontageSectionChanged(CurrentComboIndex);
		StartComboTimer(MontageSection);
		OnAttackStart();
		bHasNextComboInput = false;
		bAllowedInput = false;

	}
}

void UBattleGameplayAbility_ComboAttack::AllowInput()
{
	ComboAllowedTimerHandle.Invalidate();
	bAllowedInput = true;
}

void UBattleGameplayAbility_ComboAttack::OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& InData,
	FGameplayTag ApplicationTag)
{
	Super::OnTargetDataReadyCallback(InData, ApplicationTag);
	
	OnTargetDataReady(InData);
	
}

void UBattleGameplayAbility_ComboAttack::ServerRPCMontageSectionChanged_Implementation(uint8 InCurrentComboIndex)
{
	CurrentComboIndex = InCurrentComboIndex;
}

void UBattleGameplayAbility_ComboAttack::OnCompleted()
{
	
	if (bHasNextComboInput)
	{
		//UE_LOG(LogTemp, Log, TEXT("MontageEnd | bHasNextComboInput => true"));
		return;
	}

	
	Super::OnCompleted();
}

void UBattleGameplayAbility_ComboAttack::OnInterrupted()
{
	Super::OnInterrupted();
}

void UBattleGameplayAbility_ComboAttack::OnBlendOut()
{
	if (bHasNextComboInput)
	{
		//UE_LOG(LogTemp, Log, TEXT("MontageEnd | bHasNextComboInput => true"));
		return;
	}
	
	Super::OnBlendOut();
}


void UBattleGameplayAbility_ComboAttack::OnRep_HasNextComboInput()
{
}

