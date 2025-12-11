#include "BattleGameplayAbility_TryJustGuard.h"

#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "BattleActionGame/BattleLogChannels.h"
#include "BattleActionGame/Character/BattleHeroComponent.h"
#include "BattleActionGame/Combat/BattleCombatManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleGameplayAbility_TryJustGuard)

UBattleGameplayAbility_TryJustGuard::UBattleGameplayAbility_TryJustGuard(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
}

void UBattleGameplayAbility_TryJustGuard::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	bPressedKey = false;

	BA_DEFAULT_LOG(LogBattle, Log, TEXT("Start"));
	
	if (HasAuthority(&ActivationInfo))
	{
		float AllowedTime = TriggerEventData->EventMagnitude;

		if (const AActor* Instigator = TriggerEventData->Instigator)
		{
			BA_DEFAULT_LOG(LogBattle, Log, TEXT("TargetCombat Set"));
			TargetCombatManager = Cast<UBattleCombatManagerComponent>(Instigator->GetComponentByClass(UBattleCombatManagerComponent::StaticClass()));
		}

		GetWorld()->GetTimerManager().SetTimer(AbilityEndTimer,
			[this]()
			{
				this->EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
			},
			AllowedTime, false
		);
	}

	if (IsLocallyControlled())
	{
		if (UBattleHeroComponent* HeroComponent = Cast<UBattleHeroComponent>(GetAvatarActorFromActorInfo()->GetComponentByClass(UBattleHeroComponent::StaticClass())))
		{
			BA_DEFAULT_LOG(LogBattle, Log, TEXT("InputSet"));
			InputDelegateHandle = HeroComponent->SpecialKeyPressed.AddUObject(this, &ThisClass::GuardKeyPressed);
		}
		else
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		}
	}
	


	


	
}

void UBattleGameplayAbility_TryJustGuard::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (IsLocallyControlled())
	{
		if (UBattleHeroComponent* HeroComponent = Cast<UBattleHeroComponent>(GetAvatarActorFromActorInfo()->GetComponentByClass(UBattleHeroComponent::StaticClass())))
		{
			HeroComponent->SpecialKeyPressed.Remove(InputDelegateHandle);
		}
	}
}

void UBattleGameplayAbility_TryJustGuard::ServerKeyPressed_Implementation()
{
	BA_DEFAULT_LOG(LogBattle, Log, TEXT("ServerKeyPressed"));

	if (GetWorld()->GetNetMode() == NM_Client)
	{
		BA_DEFAULT_LOG(LogBattle, Log, TEXT("Client"));
	}
	else
	{
		BA_DEFAULT_LOG(LogBattle, Log, TEXT("Server"));
	}
	if (TargetCombatManager != nullptr)
	{
		TargetCombatManager->TryJustGuard(GetAvatarActorFromActorInfo());
	}
}

void UBattleGameplayAbility_TryJustGuard::GuardKeyPressed()
{
	if (!bPressedKey)
	{
		BA_DEFAULT_LOG(LogBattle, Log, TEXT("Pressed"));
		ServerKeyPressed();
		bPressedKey = true;
	}
	

}
