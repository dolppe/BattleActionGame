#include "BattleGameplayAbility_TryJustGuard.h"

#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "BattleActionGame/Character/BattleHeroComponent.h"
#include "BattleActionGame/Combat/BattleCombatManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleGameplayAbility_TryJustGuard)

UBattleGameplayAbility_TryJustGuard::UBattleGameplayAbility_TryJustGuard(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
}

void UBattleGameplayAbility_TryJustGuard::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (TriggerEventData == nullptr)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
	
	float AllowedTime = TriggerEventData->EventMagnitude;

	if (IsLocallyControlled())
	{
		bool bNeedEndAbility = false;
		if (UBattleHeroComponent* HeroComponent = Cast<UBattleHeroComponent>(GetAvatarActorFromActorInfo()->GetComponentByClass(UBattleHeroComponent::StaticClass())))
		{
			InputDelegateHandle = HeroComponent->SpecialKeyPressed.AddUObject(this, &ThisClass::GuardKeyPressed);
		}
		else
		{
			bNeedEndAbility = true;
		}

		if (const AActor* Instigator = TriggerEventData->Instigator)
		{
			TargetCombatManager = Cast<UBattleCombatManagerComponent>(Instigator->GetComponentByClass(UBattleCombatManagerComponent::StaticClass()));
		}
		else
		{
			bNeedEndAbility = true;
		}

		if (bNeedEndAbility)
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		}
	}
	


	

	GetWorld()->GetTimerManager().SetTimer(AbilityEndTimer,
		[this]()
		{
			this->EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		},
		AllowedTime, false
	);
	
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

void UBattleGameplayAbility_TryJustGuard::GuardKeyPressed()
{
	if (TargetCombatManager != nullptr)
	{
		TargetCombatManager->TryJustGuard(GetAvatarActorFromActorInfo());
	}
}
