#include "BattleHealthComponent.h"

#include "GameplayEffectExtension.h"
#include "BattleActionGame/BattleGameplayTags.h"
#include "BattleActionGame/BattleLogChannels.h"
#include "BattleActionGame/AbilitySystem/BattleAbilitySystemComponent.h"
#include "BattleActionGame/AbilitySystem/Attributes/BattleHealthSet.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleHealthComponent)

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

	return nullptr;
}

void UBattleHealthComponent::HandleHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	BA_SUBLOG(LogBattle, Log, TEXT("Health Change : %f => %f"), ChangeData.OldValue, ChangeData.NewValue);
	OnHealthChanged.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue, GetInstigatorFromAttrChangeData(ChangeData));
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
		// DeathMessage;

	}
	

#endif
	
}

void UBattleHealthComponent::OnRep_DeathState(EBattleDeathState OldDeathState)
{
}
