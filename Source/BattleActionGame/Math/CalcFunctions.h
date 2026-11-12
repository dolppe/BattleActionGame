#pragma once

#include "CalcFunctions.generated.h"

UENUM(BlueprintType)
enum class EAxisFunctionType : uint8 {
	Quadratic,
	Logistic,
	Logit
};


UCLASS()
class UCalcFunctions : public UObject
{
	GENERATED_BODY()
public:

	UCalcFunctions();
	
	static float QuadraticFunction(float X, float Slope, float Exponent, float Vertical, float Horizontal, float MinValue, float MaxValue);
	static float LogisticFunction(float X, float Slope, float Exponent, float Vertical, float Horizontal, float MinValue, float MaxValue);
	static float LogitFunction(float X, float Slope, float Exponent, float Vertical, float Horizontal, float MinValue, float MaxValue);
	static float ClampResult(float Result, float MinValue, float MaxValue);

	// X, Slope, Multiply, Vertical, Horizontal
	static float CalcValueWithFunction(EAxisFunctionType FunctionType,float X, float Slope, float Exponent, float Vertical, float Horizontal, float MinValue, float MaxValue);
	
	
	
};
