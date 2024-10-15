#include "CalcFunctions.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(CalcFunctions)

UCalcFunctions::UCalcFunctions()
{
}

float UCalcFunctions::LinearClampedFunction(float X, float Slope, float YOffSet, float MinVal, float MaxVal)
{
	return 0.0f;
}

float UCalcFunctions::SigmoidFunction(float X, float Slope)
{
	return 0.0f;
}

float UCalcFunctions::QuadraticFunction(float X, float Curvature, float Linear, float Offset)
{
	return 0.0f;
}

float UCalcFunctions::ExeponentailFunction(float X, float Base)
{
	return 0.0f;
}

TFunction<float(float)> UCalcFunctions::FindFunction(EAxisFunction AxisFunction)
{
	return nullptr;
}
