#pragma once

#include "CalcFunctions.generated.h"

UENUM(BlueprintType)
enum class EAxisFunction : uint8 {
	Linear,
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
	
	static float LinearFunction(float X, float Slope, float Exponent, float Vertical, float Horizontal);
	static float QuadraticFunction(float X, float Slope, float Exponent, float Vertical, float Horizontal);
	static float LogisticFunction(float X, float Slope, float Exponent, float Vertical, float Horizontal);
	static float LogitFunction(float X, float Slope, float Exponent, float Vertical, float Horizontal);

	// X, Slope, Multiply, Vertical, Horizontal
	static float CalcValueWithFunction(EAxisFunction FunctionType,float X, float Slope, float Exponent, float Vertical, float Horizontal);
	
	
	
};
