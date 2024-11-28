#pragma once

#include "Components/GameFrameworkComponent.h"
#include "BattleHealthComponent.generated.h"

class UBattleAbilitySystemComponent;
class UBattleHealthSet;
struct FOnAttributeChangeData;
struct FGameplayEffectSpec;

UENUM(BlueprintType)
enum class EBattleDeathState : uint8
{
	NotDead = 0,
	DeathStarted,
	DeathFinished
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBattleHealth_DeathEvent, AActor*, OwningActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FBattleHealth_AttributeChanged, UBattleHealthComponent*, HealthComponent, float, OldValue, float, NewValue, AActor*, Instigator);

UCLASS(Blueprintable, Meta=(BlueprintSpawnableComponent))
class BATTLEACTIONGAME_API UBattleHealthComponent : public UGameFrameworkComponent
{
	GENERATED_BODY()
public:

	UBattleHealthComponent(const FObjectInitializer& ObjectInitializer);
	
	void InitializeWithAbilitySystem(UBattleAbilitySystemComponent* InASC);
	void UninitializeFromAbilitySystem();

	UFUNCTION(BlueprintPure, Category="Battle|Health")
	static UBattleHealthComponent* FindHealthComponent(const AActor* Actor);
	
	UFUNCTION(BlueprintCallable, Category = "Battle|Health")
	float GetHealth() const;
	
	UFUNCTION(BlueprintCallable, Category = "Battle|Health")
	float GetMaxHealth() const;
	
	UFUNCTION(BlueprintCallable, Category = "Battle|Health")
	float GetHealthNormalized() const;

	UFUNCTION(BlueprintCallable, Category = "Battle|Health")
	float GetStamina() const;
	
	UFUNCTION(BlueprintCallable, Category = "Battle|Health")
	float GetMaxStamina() const;

	UFUNCTION(BlueprintCallable, Category = "Battle|Health")
	float GetStaminaNormalized() const;

	UFUNCTION(BlueprintCallable, Category = "Battle|Health")
	EBattleDeathState GetDeathState() const { return DeathState; }

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Battle|Health", Meta = (ExpandBoolAsExecs = "ReturnValue"))
	bool IsDeadOrDying() const { return (DeathState > EBattleDeathState::NotDead); }
	
	virtual void StartDeath();
	
	virtual void FinishDeath();
	

public:
	
	UPROPERTY(BlueprintAssignable)
	FBattleHealth_AttributeChanged OnHealthChanged;
	
	UPROPERTY(BlueprintAssignable)
	FBattleHealth_AttributeChanged OnMaxHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FBattleHealth_AttributeChanged OnStaminaChanged;
	
	UPROPERTY(BlueprintAssignable)
	FBattleHealth_AttributeChanged OnMaxStaminaChanged;
	
	UPROPERTY(BlueprintAssignable)
	FBattleHealth_DeathEvent OnDeathStarted;
	
	UPROPERTY(BlueprintAssignable)
	FBattleHealth_DeathEvent OnDeathFinished;

protected:

	virtual void OnUnregister() override;

	void ClearGameplayTags();

	virtual void HandleHealthChanged(const FOnAttributeChangeData& ChangeData);
	virtual void HandleMaxHealthChanged(const FOnAttributeChangeData& ChangeData);
	virtual void HandleStaminaChanged(const FOnAttributeChangeData& ChangeData);
	virtual void HandleMaxStaminaChanged(const FOnAttributeChangeData& ChangeData);
	virtual void HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude);

	

	UFUNCTION()
	virtual void OnRep_DeathState(EBattleDeathState OldDeathState);
	
protected:

	UPROPERTY()
	TObjectPtr<UBattleAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<const UBattleHealthSet> HealthSet;


	UPROPERTY(ReplicatedUsing = OnRep_DeathState)
	EBattleDeathState DeathState;
	
};

