#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BattleUserFacingExperienceDefinition.generated.h"

UCLASS(BlueprintType)
class BATTLEACTIONGAME_API UBattleUserFacingExperienceDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, BlueprintPure=false)
	UCommonSession_HosetSessionRequest* CreateHostingRequest() const;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Experience, meta=(AllowedTypes="Map"))
	FPrimaryAssetId MapID;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=Experience, meta=(AllowedTypes="BattleExperienceDefinition"))
	FPrimaryAssetId ExperienceID;
	
};
