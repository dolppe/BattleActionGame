#include "BattleFunctionalTestPlayerController.h"

#include "EnhancedPlayerInput.h"
#include "BattleActionGame/BattleLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleFunctionalTestPlayerController)

ABattleFunctionalTestPlayerController::ABattleFunctionalTestPlayerController(
	const FObjectInitializer& ObjectInitializer)
		: Super(ObjectInitializer)
{
	
}

PRAGMA_DISABLE_OPTIMIZATION

void ABattleFunctionalTestPlayerController::ClientRPCInputAxis_Implementation(const FKey& Key, const FVector& Delta)
{
	FInputKeyParams PressedInputKeyParams;
	PressedInputKeyParams.Key = Key;
	PressedInputKeyParams.Event = IE_Axis;
	PressedInputKeyParams.Delta  = Delta;
	PressedInputKeyParams.bIsGamepadOverride = false;
	
	InputKey(PressedInputKeyParams);
}

void ABattleFunctionalTestPlayerController::ClientRPCInjectionInputAction_Implementation(const UInputAction* InputAction)
{
	UEnhancedPlayerInput* EnhancedPlayerInput =	Cast<UEnhancedPlayerInput>(PlayerInput);

	EnhancedPlayerInput->InjectInputForAction(
		InputAction,
		FInputActionValue(true),
		{},
		{}
	);
	
}

void ABattleFunctionalTestPlayerController::RepeatTimer(const FKey& Key, float Period)
{
	if (bEndRepeat)
	{
		return;
	}
	else
	{
		FInputKeyParams PressedInputKeyParams;
		PressedInputKeyParams.Key = Key;
		PressedInputKeyParams.Event = IE_Pressed;
		PressedInputKeyParams.Delta  = FVector(1.0f,0.0f,0.0f);
		PressedInputKeyParams.bIsGamepadOverride = false;
	
		InputKey(PressedInputKeyParams);
		
		FTimerHandle TimerHandle;
		
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this, Key]()
		{
			FInputKeyParams ReleasedInputKeyParams;
			ReleasedInputKeyParams.Key = Key;
			ReleasedInputKeyParams.Event = IE_Released;
			ReleasedInputKeyParams.Delta  = FVector(0.0f,0.0f,0.0f);
			
			ReleasedInputKeyParams.bIsGamepadOverride = false;
			
			InputKey(ReleasedInputKeyParams);
		}), 0.05f,false);
		
		GetWorld()->GetTimerManager().SetTimer(RepeatTimerHandle, FTimerDelegate::CreateLambda([this, Key, Period]()
		{
			this->RepeatTimer(Key, Period);
		}),Period, false);
		
	}
	
}

void ABattleFunctionalTestPlayerController::ClientRPCInputPressRepeat_Implementation(const FKey& Key, float Duration,
                                                                                     float Period)
{
	bEndRepeat = false;
	
	RepeatTimer(Key, Period);
	
	GetWorld()->GetTimerManager().SetTimer(RepeatEndTimerHandle, FTimerDelegate::CreateLambda([this]()
	{
		this->bEndRepeat = true;
	}), Duration, false);
	
}

void ABattleFunctionalTestPlayerController::ClientRPCInputPress_Implementation(const FKey& Key)
{
	FInputKeyParams PressedInputKeyParams;
	PressedInputKeyParams.Key = Key;
	PressedInputKeyParams.Event = IE_Pressed;
	PressedInputKeyParams.Delta  = FVector(1.0f,0.0f,0.0f);
	PressedInputKeyParams.bIsGamepadOverride = false;
	
	InputKey(PressedInputKeyParams);
	
	FTimerHandle TimerHandle;
	
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this, Key]()
	{
		FInputKeyParams ReleasedInputKeyParams;
		ReleasedInputKeyParams.Key = Key;
		ReleasedInputKeyParams.Event = IE_Released;
		ReleasedInputKeyParams.Delta  = FVector(0.0f,0.0f,0.0f);
		ReleasedInputKeyParams.bIsGamepadOverride = false;
		
		InputKey(ReleasedInputKeyParams);
	}), 0.05f, false);
	
}


PRAGMA_ENABLE_OPTIMIZATION
