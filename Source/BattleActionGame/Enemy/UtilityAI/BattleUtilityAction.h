#pragma once

#include "BattleUtilityAIComponent.h"
#include "BattleUtilityAction.generated.h"

struct FConsiderationFactors;
class UBattleUtilityAxis;
struct FAxisConfig;

UCLASS()
class UBattleUtilityAction : public UObject
{
	GENERATED_BODY()
public:

	UBattleUtilityAction();

	UFUNCTION()
	virtual void InitAxis(TArray<FAxisConfig> AxisConfigs);

	UFUNCTION()
	virtual float EvaluateScore(const FConsiderationFactors& ConsiderList);

	UFUNCTION()
	virtual void StartAction();

	UFUNCTION()
	virtual void EndAction();

	UFUNCTION()
	virtual bool TickAction(float DeltaTime);

	UFUNCTION()
	virtual uint8 GetPriority()
	{
		return Priority;
	}

protected:

	// Instance화된 Axis들
	TArray<TObjectPtr<UBattleUtilityAxis>> AxisArray;

	uint8 Priority = 1;
	
	
};
