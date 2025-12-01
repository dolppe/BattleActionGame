#pragma once

#include "BattleHitMessage.generated.h"


USTRUCT(BlueprintType)
struct FBattleHitMessage
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite)
	TArray<FHitResult> HitResults;

	UPROPERTY(BlueprintReadWrite)
	int WindowIndex = 0;

	UPROPERTY(BlueprintReadWrite)
	float HitTime = 0.0f;
};

