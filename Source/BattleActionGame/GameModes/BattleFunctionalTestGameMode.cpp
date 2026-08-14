#include "BattleFunctionalTestGameMode.h"

#include "BattleGameState.h"
#include "BattleWorldSettings.h"
#include "BattleActionGame/Character/BattleCharacter.h"
#include "BattleActionGame/Player/BattleFunctionalTestPlayerController.h"
#include "BattleActionGame/Player/BattlePlayerController.h"
#include "BattleActionGame/Player/BattlePlayerState.h"
#include "BattleActionGame/UI/BattleHUD.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleFunctionalTestGameMode)

ABattleFunctionalTestGameMode::ABattleFunctionalTestGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GameStateClass = ABattleGameState::StaticClass();
	PlayerControllerClass = ABattleFunctionalTestPlayerController::StaticClass();
	PlayerStateClass = ABattlePlayerState::StaticClass();
	DefaultPawnClass = ABattleCharacter::StaticClass();
	HUDClass = ABattleHUD::StaticClass();
}
