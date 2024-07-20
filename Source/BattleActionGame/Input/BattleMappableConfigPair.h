#pragma once

#include "PlayerMappableInputConfig.h"
#include "BattleMappableConfigPair.generated.h"


USTRUCT()
struct FBattleMappableConfigPair
{
	GENERATED_BODY()
public:
	FBattleMappableConfigPair() = default;

	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UPlayerMappableInputConfig> Config;

	UPROPERTY(EditAnywhere)
	bool bShouldActivateAutomatically = true;
};
