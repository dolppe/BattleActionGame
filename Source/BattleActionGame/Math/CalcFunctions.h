#pragma once

#include "CalcFunctions.generated.h"

UENUM(BlueprintType)
enum class EAxisFunction : uint8 {
	LinearClamped,
	Sigmoid,
	Quadratic,
	Exponential
};


UCLASS()
class UCalcFunctions : public UObject
{
	GENERATED_BODY()
public:

	UCalcFunctions();
	
	float LinearClampedFunction(float X, float Slope = 1.0f, float YOffSet = 0.0f, float MinVal = 0.0f, float MaxVal = 1.0f);
	float SigmoidFunction(float X, float Slope =1.0f);
	float QuadraticFunction(float X, float Curvature = 1.0f, float Linear = 0.0f, float Offset = 0.0f);
	float ExeponentailFunction(float X, float Base = 2.0f);
	
	TFunction<float(float)> FindFunction(EAxisFunction AxisFunction);
	
	
};
