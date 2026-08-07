#include "BattleFunctionalTestPlayerController.h"

#include "BattleActionGame/BattleLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleFunctionalTestPlayerController)

ABattleFunctionalTestPlayerController::ABattleFunctionalTestPlayerController(
	const FObjectInitializer& ObjectInitializer)
		: Super(ObjectInitializer)
{
	
}

void ABattleFunctionalTestPlayerController::ClientRPCInputPress_Implementation(const FKey& Key)
{
	FInputKeyParams PressedInputKeyParams;
	PressedInputKeyParams.Key = Key;
	PressedInputKeyParams.Event = IE_Pressed;
	PressedInputKeyParams.Delta  = FVector(1.0f,0.0f,0.0f);
	PressedInputKeyParams.bIsGamepadOverride = false;
	
	InputKey(PressedInputKeyParams);
	
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this, Key]()
	{
		FInputKeyParams ReleasedInputKeyParams;
		ReleasedInputKeyParams.Key = Key;
		ReleasedInputKeyParams.Event = IE_Released;
		ReleasedInputKeyParams.Delta  = FVector(0.0f,0.0f,0.0f);
		ReleasedInputKeyParams.bIsGamepadOverride = false;
		
		InputKey(ReleasedInputKeyParams);
	}), 10.f, false);
	
}
