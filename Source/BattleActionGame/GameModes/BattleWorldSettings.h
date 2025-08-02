#pragma once

#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"
#include "BattleActionGame/Navigation/WorldRiskGridMap.h"
#include "BattleWorldSettings.generated.h"

class UBattleExperienceDefinition;

UCLASS()
class BATTLEACTIONGAME_API ABattleWorldSettings : public AWorldSettings
{
	GENERATED_BODY()

public:

	ABattleWorldSettings(const FObjectInitializer& ObjectInitializer);

	FPrimaryAssetId GetDefaultGameplayExperience() const;

	virtual void BeginPlay() override;

	const FGridMapInitData& GetGridMapInitData()
	{
		return GridMapInitData;
	}
	

protected:
	UPROPERTY(EditDefaultsOnly, Category=GameMode)
	TSoftClassPtr<UBattleExperienceDefinition> DefaultGameplayExperience;

	UPROPERTY(EditAnywhere, Category=GameMode)
	FGridMapInitData GridMapInitData;
	
	
};