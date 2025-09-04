#pragma once

#include "GameplayTagContainer.h"
#include "GameFramework/PlayerStart.h"
#include "BattlePlayerStart.generated.h"

UCLASS(Config = Game)
class BATTLEACTIONGAME_API ABattlePlayerStart : public APlayerStart
{
	GENERATED_BODY()
public:

	ABattlePlayerStart(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	FGameplayTag GetPlayerStartTag()
	{
		return PhaseTag;
	}

protected:

	UPROPERTY(EditAnywhere)
	FGameplayTag PhaseTag;
	
	
};

