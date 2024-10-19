#pragma once

#include "BattleUtilityAxis.generated.h"

enum class EAxisType : uint8;
enum class EBattleConsiderType : uint8;
enum class EAxisFunction : uint8;
class UBattleUtilityAIComponent;

UCLASS(BlueprintType)
class UBattleUtilityAxis : public UObject
{
	GENERATED_BODY()
public:

	UBattleUtilityAxis();
	
	virtual void SetConsiderFactor(EBattleConsiderType InConsiderType)
	{
		ConsiderType = InConsiderType;
	}
	
	virtual EBattleConsiderType GetConsiderFactor()
	{
		return ConsiderType;
	}
	virtual EAxisType GetAxisType()
	{
		return AxisType;
	}

	virtual void SetAIComponent(UBattleUtilityAIComponent* InAIComponent)
	{
		CachedAIComponent = InAIComponent;
	}
	
	virtual void SetFunction(EAxisFunction InFunctionType, float InSlope, float InExponent, float InVertical, float InHorizontal)
	{
		FunctionType = InFunctionType;
		Slope = InSlope;
		Exponent = InExponent;
		VerticalShift = InVertical;
		HorizontalShift = InHorizontal;
	}

	virtual float CalcValue(float InValue);

	virtual float GetValue();

protected:
	
	EBattleConsiderType ConsiderType;
	EAxisType AxisType;
	EAxisFunction FunctionType;
	
	float Slope;
	float Exponent;
	float VerticalShift;
	float HorizontalShift;

	UPROPERTY()
	TObjectPtr<UBattleUtilityAIComponent> CachedAIComponent;
	
};

UCLASS()
class UBattleUtilityArrayAxis : public UBattleUtilityAxis
{
	GENERATED_BODY()

public:

	UBattleUtilityArrayAxis();

	virtual float CalcValue(float InValue) override;
	virtual EAxisType GetAxisType() override;
	TArray<float> GetArrayValue();

protected:
	EAxisType AxisType;
	
};
