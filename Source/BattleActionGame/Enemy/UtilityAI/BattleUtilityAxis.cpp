#include "BattleUtilityAxis.h"

#include "BattleUtilityAIData.h"
#include "BattleActionGame/Math/CalcFunctions.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleUtilityAxis)

UBattleUtilityAxis::UBattleUtilityAxis()
{
}

void UBattleUtilityAxis::SetFunction(const FAxisConfig& AxisConfig)
{
	FunctionType = AxisConfig.FunctionType;
	Slope = AxisConfig.Slope;
	Exponent = AxisConfig.Exponent;
	VerticalShift = AxisConfig.VerticalShift;
	HorizontalShift = AxisConfig.HorizontalShift;
	AxisMinValue = AxisConfig.AxisMinValue;
	AxisMaxValue = AxisConfig.AxisMaxValue;
}

float UBattleUtilityAxis::CalcValue(float InValue) const
{
	return UCalcFunctions::CalcValueWithFunction(FunctionType, InValue, Slope, Exponent, VerticalShift, HorizontalShift, AxisMinValue, AxisMaxValue);
}



