#pragma once

#include "ModularAIController.h"
#include "BattleEnemyController.generated.h"

UCLASS(Blueprintable)
class ABattleEnemyController : public AModularAIController
{
	GENERATED_BODY()
public:
	ABattleEnemyController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void ServerRestartController();

	virtual void OnUnPossess() override;
	
	
};

