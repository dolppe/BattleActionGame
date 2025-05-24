#include "BattleGameplayAbility_JustClashAction.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameplayTagsManager.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "BattleActionGame/BattleGameplayTags.h"
#include "BattleActionGame/BattleLogChannels.h"
#include "BattleActionGame/Character/BattleCharacterBase.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleGameplayAbility_JustClashAction)

UBattleGameplayAbility_JustClashAction::UBattleGameplayAbility_JustClashAction(
	const FObjectInitializer& ObjectInitializer)
{
	UGameplayTagsManager::Get().CallOrRegister_OnDoneAddingNativeTagsDelegate(FSimpleDelegate::CreateUObject(this, &ThisClass::DoneAddingNativeTags));
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
}

void UBattleGameplayAbility_JustClashAction::DoneAddingNativeTags()
{
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		FAbilityTriggerData TriggerData;
		TriggerData.TriggerTag = FBattleGameplayTags::Get().Ability_Trigger_JustClash;
		TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
		AbilityTriggers.Add(TriggerData);
	}
}

PRAGMA_DISABLE_OPTIMIZATION

void UBattleGameplayAbility_JustClashAction::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                             const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                             const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	BA_DEFAULT_LOG(LogBattle, Log, TEXT("JustClashStart"));

	ABattleCharacterBase* TargetCharacter = Cast<ABattleCharacterBase>(TargetActor);
	UAbilitySystemComponent* TargetASC = TargetCharacter->GetAbilitySystemComponent();
	TargetASC->AddLooseGameplayTag(FBattleGameplayTags::Get().Ability_Trigger_JustClash);
	
}

PRAGMA_ENABLE_OPTIMIZATION

void UBattleGameplayAbility_JustClashAction::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{

	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	ABattleCharacterBase* TargetCharacter = Cast<ABattleCharacterBase>(TargetActor);
	UAbilitySystemComponent* TargetASC = TargetCharacter->GetAbilitySystemComponent();

	TargetASC->RemoveLooseGameplayTag(FBattleGameplayTags::Get().Ability_Trigger_JustClash);
	GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(FBattleGameplayTags::Get().Ability_Trigger_JustClash);

	if (GetWorld()->GetNetMode() == NM_Client)
	{
	
	}
	else
	{
		TargetCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	}
	
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UBattleGameplayAbility_JustClashAction::MoveToBestSpot()
{
	if (ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		FVector Direction = TargetActor->GetActorLocation() - Character->GetActorLocation();
		Direction.Z = 0.0f;
		if (Character->GetCharacterMovement()->bOrientRotationToMovement)
		{
			Character->GetCharacterMovement()->bOrientRotationToMovement = false;
			Character->SetActorRotation(Direction.Rotation());
			Character->GetCharacterMovement()->bOrientRotationToMovement = true;	
		}
		else
		{
			Character->SetActorRotation(Direction.Rotation());
		}
		if (AController* Controller = Character->GetController())
		{
			if (Character->bUseControllerRotationYaw)
			{
				Controller->SetControlRotation(Direction.Rotation());		
			}
		}
		
		FVector LaunchDirection = (-Direction).GetSafeNormal();
		LaunchDirection *= 1000.0f;
		LaunchDirection.Z = 500.0f;
		Character->LaunchCharacter(LaunchDirection, true, true);
		
		DrawDebugLine(GetWorld(), Direction, Character->GetActorLocation(), FColor::Red, false, 10.f, 0, 0);
		
		Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	}
	if (ACharacter* TargetCharacter = Cast<ACharacter>(TargetActor))
	{
		AActor* MyActor = GetAvatarActorFromActorInfo();
		FVector Direction = MyActor->GetActorLocation() - TargetCharacter->GetActorLocation();
		Direction.Z = 0.0f;

		if (TargetCharacter->GetCharacterMovement()->bOrientRotationToMovement)
		{
			TargetCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
			TargetCharacter->SetActorRotation(Direction.Rotation());
			TargetCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;
		}
		else
		{
			TargetCharacter->SetActorRotation(Direction.Rotation());
		}
		if (AController* Controller = TargetCharacter->GetController())
		{
			if (TargetCharacter->bUseControllerRotationYaw)
			{
				Controller->SetControlRotation(Direction.Rotation());
			}
		}
		
		FVector LaunchDirection = (-Direction).GetSafeNormal();
		LaunchDirection *= 1000.0f;
		LaunchDirection.Z = 500.0f;
		TargetCharacter->LaunchCharacter(LaunchDirection, true, true);
		
		DrawDebugLine(GetWorld(), Direction, TargetCharacter->GetActorLocation(), FColor::Blue, false, 10.f, 0, 0);
		
		TargetCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	}
	
}

void UBattleGameplayAbility_JustClashAction::PlaySpecialActionMontage()
{
	if (GetWorld()->GetNetMode() != NM_Client)
	{
		if (ABattleCharacterBase* TargetCharacter = Cast<ABattleCharacterBase>(TargetActor))
		{
			BA_DEFAULT_LOG(LogBattle, Log, TEXT("NetPlayMontage Start"));
			TargetCharacter->NetPlayMontage(TargetMontage);
		}
	}
	


	UAbilityTask_PlayMontageAndWait* PlayAttackMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("PlayMontage"), OwnerMontage, 0.65f);
	PlayAttackMontage->OnCompleted.AddDynamic(this, &UBattleGameplayAbility_JustClashAction::OnCompleted);
	PlayAttackMontage->OnInterrupted.AddDynamic(this, &UBattleGameplayAbility_JustClashAction::OnInterrupted);
	PlayAttackMontage->OnBlendOut.AddDynamic(this, &UBattleGameplayAbility_JustClashAction::OnBlendOut);
	PlayAttackMontage->ReadyForActivation();	
}

void UBattleGameplayAbility_JustClashAction::PlayStartCue()
{
	if (ClashGCN.IsValid())
	{
		FGameplayCueParameters Params;
		Params.Instigator = GetAvatarActorFromActorInfo();

		UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
		ASC->ExecuteGameplayCue(ClashGCN, Params);
	}
}

