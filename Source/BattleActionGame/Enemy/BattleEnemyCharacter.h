#pragma once

#include "AbilitySystemInterface.h"
#include "ModularCharacter.h"
#include "BattleEnemyCharacter.generated.h"

class UBattleHealthComponent;
class UBattleAbilitySystemComponent;

UCLASS()
class ABattleEnemyCharacter : public AModularCharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:

	ABattleEnemyCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UBattleAbilitySystemComponent* GetBattleAbilitySystemComponent() const;

	virtual void PostInitializeComponents() override;

protected:
	
	UPROPERTY()
	TObjectPtr<UBattleAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UBattleHealthComponent> HealthComponent;
	
	
	
};

