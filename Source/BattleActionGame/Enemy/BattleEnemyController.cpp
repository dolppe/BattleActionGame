#include "BattleEnemyController.h"

#include "BattleActionGame/GameModes/BattleGameMode.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleEnemyController)

ABattleEnemyController::ABattleEnemyController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bWantsPlayerState = false;
	bStopAILogicOnUnposses = false;
	
}

void ABattleEnemyController::ServerRestartController()
{
	if (GetNetMode() == NM_Client)
	{
		return;
	}
	
	if (IsInState(NAME_Inactive) || IsInState(NAME_Spectating))
	{
		ABattleGameMode* const GameMode = GetWorld()->GetAuthGameMode<ABattleGameMode>();
	
		if (GameMode == nullptr || !GameMode->ControllerCanRestart(this))
		{
			return;
		}
	
		if (GetPawn() != nullptr)
		{
			UnPossess();
		}
	
		ResetIgnoreInputFlags();
	
		GameMode->RestartPlayer(this);
	}
}

void ABattleEnemyController::OnUnPossess()
{	
	Super::OnUnPossess();
}
