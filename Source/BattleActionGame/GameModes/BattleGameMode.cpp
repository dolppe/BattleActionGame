#include "BattleGameMode.h"

#include "BattleGameState.h"
#include "BattleActionGame/Character/BattleCharacter.h"
#include "BattleActionGame/Player/BattlePlayerController.h"
#include "BattleActionGame/Player/BattlePlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleGameMode)

ABattleGameMode::ABattleGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GameStateClass = ABattleGameState::StaticClass();
	PlayerControllerClass = ABattlePlayerController::StaticClass();
	PlayerStateClass = ABattlePlayerState::StaticClass();
	DefaultPawnClass = ABattleCharacter::StaticClass();
}
