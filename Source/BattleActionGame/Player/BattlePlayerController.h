#pragma once

#include "ModularPlayerController.h"
#include "GameFramework/PlayerController.h"
#include "BattlePlayerController.generated.h"

UCLASS()
class ABattlePlayerController : public AModularPlayerController
{
	GENERATED_BODY()

public:
	ABattlePlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	
	
};

