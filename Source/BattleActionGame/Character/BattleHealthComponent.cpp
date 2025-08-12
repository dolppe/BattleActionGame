#include "BattleHealthComponent.h"

#include "GameplayEffectExtension.h"
#include "GameplayMessageSubsystem.h"
#include "BattleActionGame/BattleGameplayTags.h"
#include "BattleActionGame/BattleLogChannels.h"
#include "BattleActionGame/AbilitySystem/BattleAbilitySystemComponent.h"
#include "BattleActionGame/AbilitySystem/BattleAbilitySystemGlobals.h"
#include "BattleActionGame/AbilitySystem/Attributes/BattleHealthSet.h"
#include "BattleActionGame/Messages/BattleVerbMessage.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleHealthComponent)

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Gameplay_Message_OutOfHealth, "Gameplay.Message.OutOfHealth");

UBattleHealthComponent::UBattleHealthComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);

	AbilitySystemComponent = nullptr;
	HealthSet = nullptr;
	DeathState = EBattleDeathState::NotDead;
}

void UBattleHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UBattleHealthComponent, DeathState);
}

void UBattleHealthComponent::InitializeWithAbilitySystem(UBattleAbilitySystemComponent* InASC)
{
	AActor* Owner = GetOwner();
	check(Owner);

	if (AbilitySystemComponent)
	{
		UE_LOG(LogBattle, Error, TEXT("UBattleHealthComponent: Health component for owner [%s] has already been initialized with an ability system."), *GetNameSafe(Owner));
		return;
	}

	AbilitySystemComponent = InASC;
	if (!AbilitySystemComponent)
	{
		UE_LOG(LogBattle, Error, TEXT("UBattleHealthComponent: Cannot initialize health component for owner [%s] with NULL ability system."), *GetNameSafe(Owner));
		return;
	}

	HealthSet = AbilitySystemComponent->GetSet<UBattleHealthSet>();
	if (!HealthSet)
	{
		UE_LOG(LogBattle, Error, TEXT("UBattleHealthComponent: Cannot initialize health component for owner [%s] with NULL health set on the ability system."), *GetNameSafe(Owner));
		return;
	}

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UBattleHealthSet::GetHealthAttribute()).AddUObject(this, &UBattleHealthComponent::HandleHealthChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UBattleHealthSet::GetMaxHealthAttribute()).AddUObject(this, &UBattleHealthComponent::HandleMaxHealthChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UBattleHealthSet::GetStaminaAttribute()).AddUObject(this, &UBattleHealthComponent::HandleStaminaChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UBattleHealthSet::GetMaxStaminaAttribute()).AddUObject(this, &UBattleHealthComponent::HandleMaxStaminaChanged);
	HealthSet->OnOutOfHealth.AddUObject(this, &UBattleHealthComponent::HandleOutOfHealth);

	AbilitySystemComponent->SetNumericAttributeBase(UBattleHealthSet::GetHealthAttribute(), HealthSet->GetMaxHealth());

	ClearGameplayTags();
	
	OnHealthChanged.Broadcast(this, HealthSet->GetHealth(), HealthSet->GetHealth(), nullptr);
	OnMaxHealthChanged.Broadcast(this, HealthSet->GetMaxHealth(), HealthSet->GetMaxHealth(), nullptr);
	OnStaminaChanged.Broadcast(this, HealthSet->GetStamina(), HealthSet->GetStamina(), nullptr);
	OnMaxStaminaChanged.Broadcast(this, HealthSet->GetMaxStamina(), HealthSet->GetMaxStamina(), nullptr);
}

void UBattleHealthComponent::UninitializeFromAbilitySystem()
{
	ClearGameplayTags();
	
	if (HealthSet)
	{
		HealthSet->OnOutOfHealth.RemoveAll(this);
	}

	HealthSet = nullptr;
	AbilitySystemComponent = nullptr;
}

UBattleHealthComponent* UBattleHealthComponent::FindHealthComponent(const AActor* Actor)
{
	if (!Actor)
	{
		return nullptr;
	}

	UBattleHealthComponent* HealthComponent = Actor->FindComponentByClass<UBattleHealthComponent>();
	return HealthComponent;
}

float UBattleHealthComponent::GetHealth() const
{
	return (HealthSet ? HealthSet->GetHealth() : 0.0f);
}

float UBattleHealthComponent::GetMaxHealth() const
{
	return (HealthSet ? HealthSet->GetMaxHealth() : 0.0f);
}

float UBattleHealthComponent::GetHealthNormalized() const
{
	if (HealthSet)
	{
		const float Health = HealthSet->GetHealth();
		const float MaxHealth = HealthSet->GetMaxHealth();

		return ((MaxHealth > 0.0f) ? (Health / MaxHealth) : 0.0f);
	}

	return 0.0f;
}

float UBattleHealthComponent::GetStamina() const
{
	return (HealthSet ? HealthSet->GetStamina() : 0.0f);
}

float UBattleHealthComponent::GetMaxStamina() const
{
	return (HealthSet ? HealthSet->GetMaxStamina() : 0.0f);
}

float UBattleHealthComponent::GetStaminaNormalized() const
{
	if (HealthSet)
	{
		const float Stamina = HealthSet->GetStamina();
		const float MaxStamina = HealthSet->GetMaxStamina();

		return ((MaxStamina > 0.0f) ? (Stamina / MaxStamina) : 0.0f);
	}
	
	return 0.0f;
}

void UBattleHealthComponent::StartDeath()
{
	if (DeathState != EBattleDeathState::NotDead)
	{
		return;
	}

	DeathState = EBattleDeathState::DeathStarted;

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->SetLooseGameplayTagCount(FBattleGameplayTags::Get().Status_Death_Dying,1);
	}

	AActor* Owner = GetOwner();
	check(Owner);

	OnDeathStarted.Broadcast(Owner);

	Owner->ForceNetUpdate();
}

void UBattleHealthComponent::FinishDeath()
{
	if (DeathState != EBattleDeathState::DeathStarted)
	{
		return;
	}

	DeathState = EBattleDeathState::DeathFinished;

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->SetLooseGameplayTagCount(FBattleGameplayTags::Get().Status_Death_Dead, 1);
	}

	AActor* Owner = GetOwner();
	check(Owner);

	OnDeathFinished.Broadcast(Owner);

	Owner->ForceNetUpdate();
}

void UBattleHealthComponent::OnUnregister()
{
	UninitializeFromAbilitySystem();
	
	Super::OnUnregister();
}

void UBattleHealthComponent::ClearGameplayTags()
{
	if (AbilitySystemComponent)
	{
		const FBattleGameplayTags& GameplayTags = FBattleGameplayTags::Get();

		AbilitySystemComponent->SetLooseGameplayTagCount(GameplayTags.Status_Death_Dying, 0);
		AbilitySystemComponent->SetLooseGameplayTagCount(GameplayTags.Status_Death_Dead, 0);
	}
}

static AActor* GetInstigatorFromAttrChangeData(const FOnAttributeChangeData& ChangeData)
{
	if (ChangeData.GEModData != nullptr)
	{
		const FGameplayEffectContextHandle& EffectContext = ChangeData.GEModData->EffectSpec.GetEffectContext();
		return EffectContext.GetOriginalInstigator();
	}
	else
	{
		const FGameplayEffectSpec* EffectSpec = UBattleAbilitySystemGlobals::GetBattleGlobals().GetCurrentAppliedGE();
		if (EffectSpec)
		{
			return EffectSpec->GetEffectContext().GetOriginalInstigator();
		}
		return nullptr;
	}
}

void UBattleHealthComponent::HandleHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	AActor* Instigator = GetInstigatorFromAttrChangeData(ChangeData);
	OnHealthChanged.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue, Instigator);
}

void UBattleHealthComponent::HandleMaxHealthChanged(const FOnAttributeChangeData& ChangeData)
{

	OnMaxHealthChanged.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue, GetInstigatorFromAttrChangeData(ChangeData));
}

void UBattleHealthComponent::HandleStaminaChanged(const FOnAttributeChangeData& ChangeData)
{

	OnStaminaChanged.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue, GetInstigatorFromAttrChangeData(ChangeData));
}

void UBattleHealthComponent::HandleMaxStaminaChanged(const FOnAttributeChangeData& ChangeData)
{

	OnMaxStaminaChanged.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue, GetInstigatorFromAttrChangeData(ChangeData));
}

void UBattleHealthComponent::HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser,
	const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude)
{
#if WITH_SERVER_CODE
	if (AbilitySystemComponent)
	{
		{
			FGameplayEventData Payload;
			Payload.EventTag = FBattleGameplayTags::Get().GameplayEvent_Death;
			Payload.Instigator = DamageInstigator;
			Payload.Target =AbilitySystemComponent->GetAvatarActor();
			Payload.OptionalObject = DamageEffectSpec.Def;
			Payload.ContextHandle = DamageEffectSpec.GetEffectContext();
			Payload.InstigatorTags = *DamageEffectSpec.CapturedSourceTags.GetAggregatedTags();
			Payload.TargetTags = *DamageEffectSpec.CapturedTargetTags.GetAggregatedTags();
			Payload.EventMagnitude = DamageMagnitude;

			FScopedPredictionWindow NewScopedWindow(AbilitySystemComponent, true);
			AbilitySystemComponent->HandleGameplayEvent(Payload.EventTag, &Payload);
		}

		{
			FBattleVerbMessage Message;
			Message.Verb = TAG_Gameplay_Message_OutOfHealth;
			Message.Instigator = DamageInstigator;
			Message.InstigatorTags = *DamageEffectSpec.CapturedSourceTags.GetAggregatedTags();
			Message.Target = AbilitySystemComponent->GetAvatarActor();
			Message.TargetTags = *DamageEffectSpec.CapturedTargetTags.GetAggregatedTags();

			UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(GetWorld());
			MessageSubsystem.BroadcastMessage(Message.Verb, Message);
		}

	}
	

#endif
	
}

void UBattleHealthComponent::OnRep_DeathState(EBattleDeathState OldDeathState)
{
}
