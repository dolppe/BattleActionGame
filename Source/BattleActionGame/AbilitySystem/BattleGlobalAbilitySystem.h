#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "GameplayAbilitySpec.h"


#include "BattleGlobalAbilitySystem.generated.h"

class UGameplayAbility;
class UGameplayEffect;
class UBattleAbilitySystemComponent;


USTRUCT()
struct FGlobalAppliedAbilityList
{
	GENERATED_BODY()

	TMap<TObjectPtr<UBattleAbilitySystemComponent>, FGameplayAbilitySpecHandle> Handles;

	void AddToASC(TSubclassOf<UGameplayAbility> Ability, UBattleAbilitySystemComponent* ASC);
	void RemoveFromASC(UBattleAbilitySystemComponent* ASC);
	void RemoveFromAll();
	
};

USTRUCT()
struct FGlobalAppliedEffectList
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<TObjectPtr<UBattleAbilitySystemComponent>, FActiveGameplayEffectHandle> Handles;

	void AddToASC(TSubclassOf<UGameplayEffect> Effect, UBattleAbilitySystemComponent* ASC);
	void RemoveFromASC(UBattleAbilitySystemComponent* ASC);
	void RemoveFromAll();
};


UCLASS()
class UBattleGlobalAbilitySystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:

	UBattleGlobalAbilitySystem();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Battle")
	void TryAbilityToAll(TSubclassOf<UGameplayAbility> Ability);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Battle")
	void GiveAbilityToAll(TSubclassOf<UGameplayAbility> Ability);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Battle")
	void RemoveAbilityFromAll(TSubclassOf<UGameplayAbility> Ability);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Battle")
	void ApplyEffectToAll(TSubclassOf<UGameplayEffect> Effect);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category="Battle")
	void RemoveEffectFromAll(TSubclassOf<UGameplayEffect> Effect);

	void RegisterASC(UBattleAbilitySystemComponent* ASC);

	void UnregisterASC(UBattleAbilitySystemComponent* ASC);

private:

	UPROPERTY()
	TMap<TSubclassOf<UGameplayAbility>, FGlobalAppliedAbilityList> AppliedAbilities;

	UPROPERTY()
	TMap<TSubclassOf<UGameplayEffect>, FGlobalAppliedEffectList> AppliedEffects;

	UPROPERTY()
	TArray<TObjectPtr<UBattleAbilitySystemComponent>> RegisteredASCs;
	
};


