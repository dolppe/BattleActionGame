#pragma once

#include "BattleActionGame/Enemy/UtilityAI/BattleConsiderationFactors.h"
#include "BattleActionGame/Enemy/UtilityAI/BattleUtilityAxis.h"
#include "BattleActionGame/Enemy/UtilityAI/Actions/BattleUtilityAction.h"
#include "UtilityAISystemTests.generated.h"

UCLASS()
class UMockConsiderationFactors : public UConsiderationFactors
{
	GENERATED_BODY()
public:
	
	void ChangeFactorValue(EBattleConsiderType Consider, float InFactorValue);
	void ChangeMultiFactorValue(EBattleConsiderType Consider, TArray<float> InMultiFactorValues);
	
};

UCLASS()
class UMockTestUtilityAxis : public UBattleUtilityAxis
{
	GENERATED_BODY()
public:
	
	
	virtual float CalcValue(float InValue) const override 
	{
		return InValue;
	}
};


UCLASS()
class UMockTestUtilityAction : public UBattleUtilityAction
{
	GENERATED_BODY()
public:
	
	void SetResultScore(float InResultScore)
	{
		ResultScore = InResultScore;
	}
	
	// 테스트용이기에 지정한 점수를 반환하며, -1이면 실제 점수 계산 진행
	virtual float EvaluateScore() override
	{
		if (ResultScore == -1.f)
		{
			return Super::EvaluateScore();
		}
		return ResultScore;
	}
	
	float ResultScore = 1.0f;
	
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
	
	TSet<EAxisType>& GetAxisTypeArray()
	{
		return AxisTypes;
	}
	
	TArray<TObjectPtr<UBattleUtilityAxis>>& GetAxisArray()
	{
		return SingleAxisList;
	}
	
	TArray<TObjectPtr<UBattleUtilityAxis>>& GetMultiAxisArray()
	{
		return MultiAxisList;
	}
	
	
	AActor* GetBestTarget(EAxisType AxisType)
	{
		const TObjectPtr<AActor>* Target = BestTargets.Find(AxisType);
		return Target ? Target->Get() : nullptr;
	}
	
	
	
	
	
};

