#pragma once

#include "BattlePlayerController.h"
#include "BattleFunctionalTestPlayerController.generated.h"

UCLASS()
class ABattleFunctionalTestPlayerController : public ABattlePlayerController 
{
	GENERATED_BODY()
public:
	
	ABattleFunctionalTestPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	UFUNCTION(Client, reliable)
	void ClientRPCInputPress(const FKey& Key);
	
	
private:
	FTimerHandle TimerHandle;
	
};

