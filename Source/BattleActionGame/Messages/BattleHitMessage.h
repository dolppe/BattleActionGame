#pragma once

#include "BattleHitMessage.generated.h"


USTRUCT(BlueprintType)
struct FBattleHitMessage
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite)
	TArray<FHitResult> HitResults;

	int WindowIndex = 0;

	float HitTime = 0.0f;
};

