#pragma once

#include "AbilitySystemInterface.h"
#include "BattleActionGame/Character/BattleCharacterBase.h"
#include "BattleEnemyCharacter.generated.h"

class UBattleHealthComponent;
class UBattleAbilitySystemComponent;

UCLASS()
class ABattleEnemyCharacter : public ABattleCharacterBase
{
	GENERATED_BODY()
public:

	ABattleEnemyCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual UBattleAbilitySystemComponent* GetBattleAbilitySystemComponent() const override;

	virtual void PostInitializeComponents() override;

protected:
	
	UPROPERTY()
	TObjectPtr<UBattleAbilitySystemComponent> AbilitySystemComponent;
	
	
};

