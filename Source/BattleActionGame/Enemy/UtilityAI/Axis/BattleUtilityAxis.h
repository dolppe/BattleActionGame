#pragma once

#include "BattleUtilityAxis.generated.h"

enum class EBattleConsiderType : uint8;
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
	virtual void SetConsiderFactor(EBattleConsiderType InAxisType)
	{
		AxisType = InAxisType;
	}

	virtual float CalcValue(float InValue);
	
	virtual EBattleConsiderType GetConsiderFactor()
	{
		return AxisType;
	}

	virtual float GetValue();
	

protected:

	float Weight;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Axis")
	TObjectPtr<UCurveFloat> CurveMapping;

	UPROPERTY()
	EBattleConsiderType AxisType;
	
};
