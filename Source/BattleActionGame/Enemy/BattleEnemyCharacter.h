#pragma once

#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "BattleActionGame/Environment/BattleUtilitySpot.h"
#include "BattleActionGame/Character/BattleCharacterBase.h"
#include "BattleEnemyCharacter.generated.h"

class ABattleUtilitySpot;
class UBattleEnemyData;
class UBattleHealthComponent;
class UBattleAbilitySystemComponent;
struct FGameplayEffectSpec;

USTRUCT()
struct FBreakablePart
{
	GENERATED_BODY()

	FBreakablePart()
	{
		
	}
	
	FBreakablePart(int InRemainHp, const TFunction<void(FGameplayTag)>& InDestroyFunction)
		: RemainHp(InRemainHp), DestroyFunction(InDestroyFunction)
	{
		
	}
	int RemainHp;
	TFunction<void(FGameplayTag)> DestroyFunction; 
	
};


UCLASS()
class ABattleEnemyCharacter : public ABattleCharacterBase
{
	GENERATED_BODY()
public:

	ABattleEnemyCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual UBattleAbilitySystemComponent* GetBattleAbilitySystemComponent() const override;

	virtual UBattleHealthComponent* GetHealthComponent() const override;

	virtual void PostInitializeComponents() override;

	UFUNCTION(BlueprintCallable)
	void AttackBreakablePart(FGameplayTag InGameplayTag);

	void HandleGroggyState(AActor* GEInstigator, AActor* GECauser, const FGameplayEffectSpec& GEEffectSpec, float GEMagnitude);

protected:
	
	UPROPERTY()
	TObjectPtr<UBattleAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UBattleEnemyData> EnemyData;

	TMap<FGameplayTag,FBreakablePart> BreakableParts;
	
	
};

