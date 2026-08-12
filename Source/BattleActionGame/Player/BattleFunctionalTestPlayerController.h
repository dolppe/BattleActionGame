#pragma once

#include "BattlePlayerController.h"
#include "InputAction.h"
#include "BattleFunctionalTestPlayerController.generated.h"

UCLASS()
class ABattleFunctionalTestPlayerController : public ABattlePlayerController 
{
	GENERATED_BODY()
public:
	
	ABattleFunctionalTestPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	UFUNCTION(Client, Reliable)
	void ClientRPCInputPress(const FKey& Key);
	
	UFUNCTION(Client, Reliable)
	void ClientRPCInputPressRepeat(const FKey& Key, float Duration, float Period);
	
	UFUNCTION(Client, Reliable)
	void ClientRPCInputAxis(const FKey& Key, const FVector& Delta);
	
	UFUNCTION(Client, Reliable)
	void ClientRPCInjectionInputAction(const UInputAction* InputAction);

	
protected:
	
	void RepeatTimer(const FKey& Key, float Period);
	
	bool bEndRepeat = false;
	
	FTimerHandle RepeatTimerHandle;
	FTimerHandle RepeatEndTimerHandle;
	
};

