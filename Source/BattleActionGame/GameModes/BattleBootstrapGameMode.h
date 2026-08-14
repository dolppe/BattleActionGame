#pragma once
#include "GameFramework/GameModeBase.h"

#include "BattleBootstrapGameMode.generated.h"


UCLASS()
class ABattleBootstrapGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	
	ABattleBootstrapGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	
	void TryMapLoad();
};
