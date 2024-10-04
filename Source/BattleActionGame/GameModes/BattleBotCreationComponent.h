#pragma once

#include "CoreMinimal.h"
#include "Components/GameStateComponent.h"

#include "BattleBotCreationComponent.generated.h"


class ABattleEnemyController;
class ABattleEnemyCharacter;
class UBattleExperienceDefinition;

UCLASS(Blueprintable, Abstract)
class UBattleBotCreationComponent : public UGameStateComponent
{
	GENERATED_BODY()
public:

	UBattleBotCreationComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable, Category="Battle|Bot")
	virtual void SpawnOneBot(TSubclassOf<ABattleEnemyCharacter> EnemyClass, TSubclassOf<ABattleEnemyController> EnemyControllerClass);


private:
	void OnExperienceLoaded(const UBattleExperienceDefinition* Experience);

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ABattleEnemyCharacter> DefaultEnemyClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ABattleEnemyController> DefaultEnemyControllerClass;
	
};

