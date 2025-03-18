#pragma once

#include "AbilitySystemInterface.h"
#include "ModularGameState.h"
#include "GameFramework/GameStateBase.h"
#include "BattleGameState.generated.h"

class UBattleExperienceManagerComponent;
class UBattleAbilitySystemComponent;

UCLASS()
class ABattleGameState : public AModularGameStateBase, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:

	ABattleGameState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void PostInitializeComponents() override;
	
	//~IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~End of IAbilitySystemInterface

	UFUNCTION(BlueprintCallable, Category = "Battle|GameState")
	UBattleAbilitySystemComponent* GetBattleAbilitySystemComponent() const { return AbilitySystemComponent; }

private:
	UPROPERTY()
	TObjectPtr<UBattleExperienceManagerComponent> ExperienceManagerComponent;

	UPROPERTY(VisibleAnywhere, Category = "Battle|GameState")
	TObjectPtr<UBattleAbilitySystemComponent> AbilitySystemComponent;
	
};
