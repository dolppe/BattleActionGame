#pragma once

#include "UObject/Object.h"
#include "BattleUtilityAxis.generated.h"

struct FAxisConfig;
enum class EBattleConsiderType : uint8;
enum class EAxisFunctionType : uint8;

UENUM(BlueprintType)
enum class EAxisType : uint8
{
	None,
	Single,
	Target,
	
};

/*
 *	Axis에 해당하는 타입과 함수 설정을 관리
 */
UCLASS(BlueprintType)
class UBattleUtilityAxis : public UObject
{
	GENERATED_BODY()
public:

	UBattleUtilityAxis();
	
	// 함수 세팅
	void SetFunction(const FAxisConfig& AxisConfig);

	// 전달된 값을 X값으로 사용하여 설정된 함수의 Y값을 반환
	virtual float CalcValue(float InValue) const;
	
	void SetConsiderType(EBattleConsiderType InConsiderType)
	{
		ConsiderType = InConsiderType;
	}
	
	EBattleConsiderType GetConsiderType() const
	{
		return ConsiderType;
	}

private:
	
	EBattleConsiderType ConsiderType;
	
	EAxisFunctionType FunctionType;
	float Slope;
	float Exponent;
	float VerticalShift;
	float HorizontalShift;
	float AxisMinValue;
	float AxisMaxValue;
	
};
