#include "CalcFunctions.h"

#include "BattleActionGame/BattleLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CalcFunctions)

UCalcFunctions::UCalcFunctions()
{
}

PRAGMA_DISABLE_OPTIMIZATION

float UCalcFunctions::LinearFunction(float X, float Slope, float Exponent, float Vertical, float Horizontal, float MinValue, float MaxValue)
{
	float Result = Slope * (X-Horizontal) + Vertical;
	Result = ClampResult(Result, MinValue, MaxValue);
	
	return Result;
}

float UCalcFunctions::QuadraticFunction(float X, float Slope, float Exponent, float Vertical, float Horizontal, float MinValue, float MaxValue)
{
	float Result = Slope * pow(X - Horizontal, Exponent) + Vertical;
	Result = ClampResult(Result, MinValue, MaxValue);
	
	return Result;
}

float UCalcFunctions::LogisticFunction(float X, float Slope, float Exponent, float Vertical, float Horizontal, float MinValue, float MaxValue)
{
	float Result = Exponent / (1.0f + FMath::Exp(-Slope * (X - Horizontal))) + Vertical;
	Result = ClampResult(Result, MinValue, MaxValue);

	return Result;
	
}

float UCalcFunctions::LogitFunction(float X, float Slope, float Exponent, float Vertical, float Horizontal, float MinValue, float MaxValue)
{
	if (Exponent - (X-Horizontal) == 0 || Exponent <= 0.001f)
	{
		UE_LOG(LogBattle, Warning, TEXT("LogitFunction Setting Error"));
		return 0.0f;
	}
	float ShiftedX = X - Horizontal;
	
	float Result = Slope * FMath::Loge(ShiftedX / (Exponent - ShiftedX)) + Vertical;
	
	Result = ClampResult(Result, MinValue, MaxValue);
	
	return Result;
	
}

float UCalcFunctions::ClampResult(float Result, float MinValue, float MaxValue)
{
	float ClampResult;
	
	if (MinValue < 0.0f && MaxValue > 1.0f)
	{
		ClampResult = FMath::Clamp(Result, 0.0f, 1.0f);
	}
	else if (MinValue <0.0f)
	{
		ClampResult = FMath::Clamp(Result, 0.0f, MaxValue);
	}
	else if (MaxValue > 1.0f)
	{
		ClampResult = FMath::Clamp(Result, MinValue, 1.0f);
	}
	else
	{
		ClampResult = FMath::Clamp(Result, MinValue, MaxValue);
	}
	
	return ClampResult;

}

float UCalcFunctions::CalcValueWithFunction(EAxisFunction FunctionType, float X, float Slope, float Exponent, float Vertical, float Horizontal, float MinValue, float MaxValue)
{
	switch (FunctionType)
	{
	case EAxisFunction::Linear:
		return LinearFunction(X, Slope, Exponent, Vertical, Horizontal, MinValue, MaxValue);
	case EAxisFunction::Quadratic:
		return QuadraticFunction(X, Slope, Exponent, Vertical, Horizontal, MinValue, MaxValue);
	case EAxisFunction::Logistic:
		return LogisticFunction(X, Slope, Exponent, Vertical, Horizontal, MinValue, MaxValue);
	case EAxisFunction::Logit:
		return LogitFunction(X, Slope, Exponent, Vertical, Horizontal, MinValue, MaxValue);
	}

	return 0.0f;
}


PRAGMA_ENABLE_OPTIMIZATION