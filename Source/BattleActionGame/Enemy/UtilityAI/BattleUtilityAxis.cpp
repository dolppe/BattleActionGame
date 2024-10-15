#include "BattleUtilityAxis.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleUtilityAxis)

UBattleUtilityAxis::UBattleUtilityAxis()
{
}

float UBattleUtilityAxis::CalcValue(float InValue)
{
	float ApplyValue = CurveMapping->GetFloatValue(InValue);
	
	return ApplyValue * Weight;
}
