#pragma once

#include "ModularGameState.h"
#include "GameFramework/GameStateBase.h"
#include "BattleGameState.generated.h"

UCLASS()
class ABattleGameState : public AModularGameStateBase
{
	GENERATED_BODY()
public:

	ABattleGameState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	
};
