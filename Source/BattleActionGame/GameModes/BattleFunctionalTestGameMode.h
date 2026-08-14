#pragma once

#include "BattleGameMode.h"
#include "GameFramework/GameModeBase.h"
#include "BattleFunctionalTestGameMode.generated.h"

UCLASS()
class ABattleFunctionalTestGameMode : public ABattleGameMode
{
	GENERATED_BODY()
public:
	
	ABattleFunctionalTestGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

};

