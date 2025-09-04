#pragma once

#include "GameplayTagContainer.h"
#include "BattleInteractionDurationMessage.generated.h"


USTRUCT(BlueprintType)
struct FBattleInteractionDurationMessage
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> Instigator = nullptr;

	UPROPERTY(BlueprintReadWrite)
	float Duration = 0;
	
};

