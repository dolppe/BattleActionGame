#pragma once

#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"
#include "BattleWorldSettings.generated.h"

class UBattleExperienceDefinition;

UCLASS()
class BATTLEACTIONGAME_API ABattleWorldSettings : public AWorldSettings
{
	GENERATED_BODY()

public:

	ABattleWorldSettings(const FObjectInitializer& ObjectInitializer);

	FPrimaryAssetId GetDefaultGameplayExperience() const;

protected:
	UPROPERTY(EditDefaultsOnly, Category=GameMode)
	TSoftClassPtr<UBattleExperienceDefinition> DefaultGameplayExperience;
	
	
};