#include "BattleGameplayAbility_Special_Spawn.h"
#include "BattleCombatManagerComponent.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "BattleActionGame/BattleGameplayTags.h"
#include "BattleActionGame/Character/BattleCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleGameplayAbility_Special_Spawn)

UBattleGameplayAbility_Special_Spawn::UBattleGameplayAbility_Special_Spawn(const FObjectInitializer& ObjectInitializer)
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
}

void UBattleGameplayAbility_Special_Spawn::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	const ABattleCharacterBase* Character = Cast<ABattleCharacterBase>(ActorInfo->AvatarActor);
	UBattleCombatManagerComponent* CurrentCombatManager = CastChecked<UBattleCombatManagerComponent>(Character->GetComponentByClass(UBattleCombatManagerComponent::StaticClass()));
	
	CurrentSpawnData = &CurrentCombatManager->GetAttackData()->SpawnDatas[SpawnMode];
	CurrentSpawnMontage = CurrentSpawnData->Montage;
	SpawnActorClass = CurrentSpawnData->SpawnedActor;

	const FName MontageSectionName = *FString::Printf(TEXT("%s%d"), *CurrentSpawnData->MontageSectionName, 1);

	UAbilityTask_PlayMontageAndWait* PlayAttackMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("PlayMontage"), CurrentSpawnMontage, 0.65f, MontageSectionName);
	PlayAttackMontage->ReadyForActivation();

	if (Character->IsLocallyControlled())
	{
		UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());

		StartListenerHandle = MessageSystem.RegisterListener(FBattleGameplayTags::Get().Combat_Attack_Event_Start, this, &UBattleGameplayAbility_Special_Spawn::StartSpawn);
	}
	if (GetWorld()->GetNetMode() != NM_Client)
	{
		UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
		if (ASC)
		{
			ASC->AddLooseGameplayTag(FBattleGameplayTags::Get().Status_Attack_Attacking);
		}

		Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	}
}

void UBattleGameplayAbility_Special_Spawn::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	ABattleCharacterBase* Character = Cast<ABattleCharacterBase>(ActorInfo->AvatarActor);
	
	if (Character->IsLocallyControlled())
	{
		// 테스크 실행
		UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());

		if (StartListenerHandle.IsValid())
		{
			MessageSystem.UnregisterListener(StartListenerHandle);
		}
	}

	if (GetWorld()->GetNetMode() != NM_Client)
	{
		UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
		
		if (ASC)
		{
			ASC->RemoveLooseGameplayTag(FBattleGameplayTags::Get().Status_Attack_Attacking);
		}

		Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UBattleGameplayAbility_Special_Spawn::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, SpawnCenterData);
}

void UBattleGameplayAbility_Special_Spawn::OnRep_SpawnCenterData()
{
}

void UBattleGameplayAbility_Special_Spawn::StartSpawn(FGameplayTag Channel, const FBattleVerbMessage& Notification)
{
	OnSpawnStart();
	
	ACharacter* Character = Cast<ACharacter>(CurrentActorInfo->AvatarActor);

	FVector CharacterLocation = Character->GetActorLocation();
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Character; // 스폰한 액터의 소유자를 설정
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn; 

	
	for (const FVector& SpawnLocation : SpawnCenterData)
	{
		FVector Direction = (SpawnLocation - CharacterLocation).GetSafeNormal();
		
		FRotator SpawnRotation = Direction.Rotation();

		SpawnRotation.Pitch = 0.0f;
		SpawnRotation.Roll = 0.0f;
		
		GetWorld()->SpawnActor(SpawnActorClass, &SpawnLocation, &SpawnRotation, SpawnParams);
	}
	
}
