#include "CalcFunctions.h"

#include "BattleActionGame/BattleLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CalcFunctions)

UCalcFunctions::UCalcFunctions()
{
}

PRAGMA_DISABLE_OPTIMIZATION

float UCalcFunctions::LinearFunction(float X, float Slope, float Exponent, float Vertical, float Horizontal)
{
	float Result = Slope * (X-Horizontal) + Vertical;

	Result = FMath::Clamp(Result, 0.0f, 1.0f);
	
	return Result;
}

float UCalcFunctions::QuadraticFunction(float X, float Slope, float Exponent, float Vertical, float Horizontal)
{
	float Result = Slope * pow(X - Horizontal, Exponent) + Vertical;
	Result = FMath::Clamp(Result, 0.0f, 1.0f);
	
	return Result;
}

float UCalcFunctions::LogisticFunction(float X, float Slope, float Exponent, float Vertical, float Horizontal)
{
	float ExponentTemp = -1.f * Slope * X + Horizontal;
	float InDenominator = 1.f / (1.f + (1000.0f * exp(ExponentTemp)));
	float Result = Exponent * InDenominator +Vertical;

	Result = FMath::Clamp(Result, 0.0f, 1.0f);

	return Result;
	
}

float UCalcFunctions::LogitFunction(float X, float Slope, float Exponent, float Vertical, float Horizontal)
{
	if (Exponent - (X-Horizontal) == 0)
	{
		UE_LOG(LogBattle, Warning, TEXT("LogitFunction Setting Error"));
		return 0.0f;
	}
	float ShiftedX = X - Horizontal;
	
	float Result = Slope * std::log(ShiftedX / (Exponent - ShiftedX)) + Vertical;
	
	Result = FMath::Clamp(Result, 0.0f, 1.0f);
	
	return Result;
	
}

float UCalcFunctions::CalcValueWithFunction(EAxisFunction FunctionType, float X, float Slope, float Exponent, float Vertical, float Horizontal)
{
	switch (FunctionType)
	{
	case EAxisFunction::Linear:
		return LinearFunction(X, Slope, Exponent, Vertical, Horizontal);
	case EAxisFunction::Quadratic:
		return QuadraticFunction(X, Slope, Exponent, Vertical, Horizontal);
	case EAxisFunction::Logistic:
		return LogisticFunction(X, Slope, Exponent, Vertical, Horizontal);
	case EAxisFunction::Logit:
		return LogitFunction(X, Slope, Exponent, Vertical, Horizontal);
	}

	return 0.0f;
}


PRAGMA_ENABLE_OPTIMIZATION