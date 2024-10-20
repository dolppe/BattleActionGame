#include "BattleUtilityAxis.h"

#include "BattleActionGame/Enemy/UtilityAI/BattleUtilityAIComponent.h"
#include "BattleActionGame/Math/CalcFunctions.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleUtilityAxis)

UBattleUtilityAxis::UBattleUtilityAxis()
{
	AxisType = EAxisType::Single;
}

float UBattleUtilityAxis::CalcValue(float InValue)
{
	return UCalcFunctions::CalcValueWithFunction(FunctionType, InValue, Slope, Exponent, VerticalShift, HorizontalShift);
}

float UBattleUtilityAxis::GetValue()
{
	return CachedAIComponent->ConsiderList->Factors[ConsiderType];
}

UBattleUtilityArrayAxis::UBattleUtilityArrayAxis()
{
	AxisType = EAxisType::Target;
}

float UBattleUtilityArrayAxis::CalcValue(float InValue)
{
	return Super::CalcValue(InValue);
}

EAxisType UBattleUtilityArrayAxis::GetAxisType()
{
	return AxisType;
}

TArray<float> UBattleUtilityArrayAxis::GetArrayValue()
{
	return CachedAIComponent->ConsiderList->ArrayFactors[ConsiderType];
}


