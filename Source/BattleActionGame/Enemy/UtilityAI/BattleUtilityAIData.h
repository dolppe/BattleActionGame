#pragma once

#include "Engine/DataAsset.h"
#include "BattleUtilityAIData.generated.h"

class UBattleUtilityAction;
class UBattleUtilityAxis;

USTRUCT(BlueprintType)
struct FAxisConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Axis")
	TSubclassOf<UBattleUtilityAxis> AxisClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Axis")
	float DefaultWeight;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Axis")
	TObjectPtr<UCurveFloat> CurveMapping;
	
};

USTRUCT(BlueprintType)
struct FActionConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action")
	TSubclassOf<UBattleUtilityAction> ActionClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action")
	TArray<FAxisConfig> AxisConfigs;
	
};



UCLASS(BlueprintType)
class UBattleUtilityAIData : public UDataAsset
{
	GENERATED_BODY()
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UtilityAI")
	TArray<FActionConfig> ActionConfigs;

};

