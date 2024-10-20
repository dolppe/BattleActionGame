#pragma once

#include "Engine/DataAsset.h"
#include "BattleUtilityAIData.generated.h"

enum class EAxisFunction : uint8;
enum class EBattleConsiderType : uint8;
enum class EAxisType : uint8;
class UBattleUtilityAction;
class UBattleUtilityAxis;



USTRUCT(BlueprintType)
struct FAxisConfig
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Axis")
	EAxisType AxisType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Axis")
	EBattleConsiderType ConsiderType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Axis")
	EAxisFunction FunctionType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Axis")
	float Slope = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Axis")
	float Exponent = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Axis")
	float VerticalShift = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Axis")
	float HorizontalShift = 0;
	
};

USTRUCT(BlueprintType)
struct FActionConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action")
	TSubclassOf<UBattleUtilityAction> ActionClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action")
	TArray<FAxisConfig> AxisConfigs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action")
	float Weight;
	
};



UCLASS(BlueprintType)
class UBattleUtilityAIData : public UDataAsset
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UtilityAI")
	TArray<EBattleConsiderType> Consider;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UtilityAI")
	TArray<FActionConfig> ActionConfigs;

};

