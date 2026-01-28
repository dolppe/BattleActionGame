#pragma once

#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "BattleActionGame/Environment/BattleUtilitySpot.h"
#include "BattleActionGame/Character/BattleCharacterBase.h"
#include "BattleEnemyCharacter.generated.h"

class ABattleEnemyCharacter;
class ABattleUtilitySpot;
class UBattleEnemyData;
class UBattleHealthComponent;
class UBattleAbilitySystemComponent;
struct FGameplayEffectSpec;
class UBattlePartsManagerComponent;



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

	UFUNCTION(BlueprintCallable)
	void ChangePhyMatPart(FName BoneName, FGameplayTag InGameplayTag);
	
	virtual void HandleDamageToPart(FGameplayTag PartTag, const FVector& AttackDirection) override;

	UFUNCTION(BlueprintCallable)
	void DestroyParts(TArray<FName> BoneNames);

	UFUNCTION()
	virtual void OnPoiseBreak(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude);

	UFUNCTION()
	virtual void OnGroggyState(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude);

protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Battle|EnemyCharacter", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBattleAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Battle|EnemyCharacter", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBattlePartsManagerComponent> PartsManagerComponent;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UBattleEnemyData> EnemyData;

	UPROPERTY(EditAnywhere)
	TMap<FGameplayTag, UPhysicalMaterial*> PhysicMaterialWithSurface;
	
	
};

