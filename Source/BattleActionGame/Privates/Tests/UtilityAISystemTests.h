#pragma once

#include "BattleActionGame/Enemy/UtilityAI/BattleUtilityAxis.h"
#include "BattleActionGame/Enemy/UtilityAI/Actions/BattleUtilityAction.h"
#include "UtilityAISystemTests.generated.h"


UCLASS()
class UMockTestUtilityAxis : public UBattleUtilityAxis
{
	GENERATED_BODY()
public:

	virtual float GetValue() override
	{
		return 0.0f;
	}
	
	virtual float CalcValue(float InValue) override
	{
		return WantedValue;
	}
	
	
	float WantedValue = 0.0f; 
	
};

UCLASS()
class UMockTestUtilityArrayAxis : public UBattleUtilityArrayAxis
{
	GENERATED_BODY()

public:
	
	virtual TArray<float> GetArrayValue() override
	{
		return WantedValue;
	}
	virtual float CalcValue(float InValue) override
	{
		return InValue;
	}
	
	TArray<float> WantedValue;
	
	
};


UCLASS()
class UMockTestUtilityAction : public UBattleUtilityAction
{
	GENERATED_BODY()
public:
	
	virtual float EvaluateScore(const UConsiderationFactors* ConsiderList) override
	{
		if (ResultScore == -1.f)
		{
			return Super::EvaluateScore(ConsiderList);
		}
		return ResultScore;
	}
	
	void SetDistanceSourceType(EDistanceSourceType InDistanceSourceType)
	{
		DistanceSourceType = InDistanceSourceType;
	}
	
	void SetMinDistance(float InMinDistance)
	{
		MinDistance = InMinDistance;
	}
	
	void SetMaxDistance(float InMaxDistance)
	{
		MaxDistance = InMaxDistance;
	}
	
	void SetAgeTime(float InAgeTime)
	{
		AgeTime = InAgeTime;
	}
	
	void SetAgeRate(float InAgeRate)
	{
		AgeRate = InAgeRate;
	}
	
	void SetCoolTime(float InCoolTime)
	{
		CoolTime = InCoolTime;
	}
	
	TArray<EAxisType>& GetAxisTypeArray()
	{
		return AxisTypeArray;
	}
	
	TArray<TObjectPtr<UBattleUtilityAxis>>& GetAxisArray()
	{
		return AxisArray;
	}
	
	TArray<TObjectPtr<UBattleUtilityArrayAxis>>& GetArrayAxisArray()
	{
		return ArrayAxisArray;
	}
	
	void SetCachedAIComponent(UBattleUtilityAIComponent* InComponent)
	{
		CachedAIComponent = InComponent;
	}
	
	TArray<TObjectPtr<AActor>>& GetBestTargets()
	{
		return BestTargets;
	}
	
	
	
	float ResultScore = 1.0f;
	
};

