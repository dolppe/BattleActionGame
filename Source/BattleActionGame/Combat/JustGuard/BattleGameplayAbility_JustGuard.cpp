#include "BattleGameplayAbility_JustGuard.h"

#include "AbilitySystemComponent.h"
#include "GameplayMessageSubsystem.h"
#include "NativeGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "BattleActionGame/BattleLogChannels.h"
#include "BattleActionGame/Character/BattleCharacterBase.h"
#include "BattleActionGame/Character/BattleHeroComponent.h"
#include "GameFramework/Character.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleGameplayAbility_JustGuard)

UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_JustGuard, "Combat.JustGuard")

UBattleGameplayAbility_JustGuard::UBattleGameplayAbility_JustGuard(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
}

void UBattleGameplayAbility_JustGuard::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	BA_DEFAULT_LOG(LogBattle,Log,TEXT("JustGuard"));

	if (IsLocallyControlled())
	{
		UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(GetWorld());

		StartListenerHandle = MessageSubsystem.RegisterListener(TAG_Gameplay_JustGuard, this, &ThisClass::StartActionTrigger);
	}
	
}

void UBattleGameplayAbility_JustGuard::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UBattleGameplayAbility_JustGuard::MoveToBestSpot()
{
	if (ACharacter* TargetPlayerCharacter = Cast<ACharacter>(TargetActor))
	{
		FVector Direction = GetAvatarActorFromActorInfo()->GetActorLocation() - TargetPlayerCharacter->GetActorLocation();
		Direction.Z = 0.0f;

		TargetPlayerCharacter->SetActorRotation(Direction.Rotation());
	}
	
}

void UBattleGameplayAbility_JustGuard::PlaySpecialActionMontage()
{
	if (GetWorld()->GetNetMode() != NM_Client)
	{
		if (ABattleCharacterBase* PlayerCharacter = Cast<ABattleCharacterBase>(TargetActor))
		{
			PlayerCharacter->NetPlayMontage(PlayerMontage);
		}
	}
}

void UBattleGameplayAbility_JustGuard::PlayStartCue()
{
	Super::PlayStartCue();
}

void UBattleGameplayAbility_JustGuard::StartActionTrigger(FGameplayTag Channel, const FBattleVerbMessage& Notification)
{
	Super::StartActionTrigger(Channel, Notification);

	if (ABattleCharacterBase* EnemyCharacter = Cast<ABattleCharacterBase>(GetAvatarActorFromActorInfo()))
	{
		EnemyCharacter->NetStopMotion(HitStopTime);
	}
	
	if (ABattleCharacterBase* PlayerCharacter = Cast<ABattleCharacterBase>(TargetActor))
	{
		PlayerCharacter->NetStopMotion(HitStopTime);

		if (UAbilitySystemComponent* PlayerASC = PlayerCharacter->GetAbilitySystemComponent())
		{
			FGameplayCueParameters Params;
			Params.Instigator = GetAvatarActorFromActorInfo();
			Params.Location = PlayerCharacter->GetMesh()->GetSocketLocation(JustGuardEffectSocketName);

			PlayerASC->ExecuteGameplayCue(JustGuardImpactGCN,Params);
		}
		
		if (UBattleHeroComponent* HeroComponent = Cast<UBattleHeroComponent>(PlayerCharacter->GetComponentByClass(UBattleHeroComponent::StaticClass())))
		{
			FVector Direction = GetAvatarActorFromActorInfo()->GetActorLocation() - PlayerCharacter->GetActorLocation();

			FVector SideDirection;

			if (Direction.Dot(PlayerCharacter->GetActorRightVector()) >=0)
			{
				SideDirection = PlayerCharacter->GetActorRightVector().GetSafeNormal();
			}
			else
			{
				SideDirection = -(PlayerCharacter->GetActorRightVector().GetSafeNormal());
			}
			
			
			FVector DesiredLocation = PlayerCharacter->GetActorLocation() - (Direction.GetSafeNormal() * 350.f) + (SideDirection*200.f);
			DesiredLocation.Z = PlayerCharacter->GetActorLocation().Z + 150.f;
			
			HeroComponent->SetDesiredCameraMode(ImpactCameraMode, DesiredLocation, (PlayerCharacter->GetActorLocation() - DesiredLocation).Rotation());
		}
	}

	GetWorld()->GetTimerManager().SetTimer(HitStopEndHandle, this, &ThisClass::OnHitStopEnd, HitStopTime, false);
	
	
}

void UBattleGameplayAbility_JustGuard::OnHitStopEnd()
{
	if (UBattleHeroComponent* HeroComponent = Cast<UBattleHeroComponent>(TargetActor->GetComponentByClass(UBattleHeroComponent::StaticClass())))
	{
		HeroComponent->ClearDesiredCameraMode();
	}

	UAbilityTask_PlayMontageAndWait* PlayAttackMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("PlayMontage"), EnemyMontage, 1.0f);
	PlayAttackMontage->OnCompleted.AddDynamic(this, &UBattleGameplayAbility_JustGuard::OnCompleted);
	PlayAttackMontage->OnInterrupted.AddDynamic(this, &UBattleGameplayAbility_JustGuard::OnInterrupted);
	PlayAttackMontage->OnBlendOut.AddDynamic(this, &UBattleGameplayAbility_JustGuard::OnBlendOut);
	PlayAttackMontage->ReadyForActivation();	

	FVector Direction = (GetAvatarActorFromActorInfo()->GetActorLocation() - TargetActor->GetActorLocation()).GetSafeNormal();
	Direction *= 1000.f;
	Direction.Z = 300.f;

	if (ABattleCharacterBase* EnemyCharacter = Cast<ABattleCharacterBase>(GetAvatarActorFromActorInfo()))
	{
		EnemyCharacter->LaunchCharacter(Direction, true, true);
	}

	Direction = -Direction;
	Direction.Z = 300.f;

	if (ABattleCharacterBase* PlayerCharacter = Cast<ABattleCharacterBase>(TargetActor))
	{
		PlayerCharacter->LaunchCharacter(Direction, true, true);
	}

	
	
}
