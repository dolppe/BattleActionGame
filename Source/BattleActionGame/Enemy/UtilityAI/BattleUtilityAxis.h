#pragma once

#include "BattleUtilityAxis.generated.h"

enum class EAxisFunction : uint8;

UCLASS()
class UBattleUtilityAxis : public UObject
{
	GENERATED_BODY()
public:

	UBattleUtilityAxis();

	virtual void SetWeight(float InWeight)
	{
		Weight = InWeight;
	}

	virtual void SetCurveData(UCurveFloat* InCurveMapping)
	{
		CurveMapping = InCurveMapping;
	}
	virtual void SetConsiderFactor(FName InConsiderFactor)
	{
		ConsiderFactor = InConsiderFactor;
	}

	virtual float CalcValue(float InValue);
	
	virtual FName GetConsiderFactor()
	{
		return ConsiderFactor;
	}
	

protected:

	float Weight;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Axis")
	TObjectPtr<UCurveFloat> CurveMapping;

	UPROPERTY()
	FName ConsiderFactor;
	
};
