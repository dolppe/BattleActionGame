#include "BattlePlayerController.h"

#include "BattleActionGame/Camera/BattlePlayerCameraManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattlePlayerController)

ABattlePlayerController::ABattlePlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PlayerCameraManagerClass = ABattlePlayerCameraManager::StaticClass();
}
