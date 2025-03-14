#pragma once

#include "BattleActionGame/Enemy/UtilityAI/BattleUtilityAIComponent.h"
#include "BattleUtilityAction.generated.h"

class UConsiderationFactors;
class UBattleUtilityAxis;
struct FAxisConfig;

UCLASS(BlueprintType, Blueprintable)
class UBattleUtilityAction : public UObject
{
	GENERATED_BODY()
public:

	UBattleUtilityAction();

	UFUNCTION()
	virtual void InitAxis(TArray<FAxisConfig> AxisConfigs, UBattleUtilityAIComponent* UtilityAIComponent);

	UFUNCTION()
	virtual float EvaluateScore(const UConsiderationFactors* ConsiderList);

	UFUNCTION()
	virtual void StartAction();

	UFUNCTION()
	virtual void EndAction();

	UFUNCTION()
	virtual bool TickAction(float DeltaTime);

	UFUNCTION(BlueprintCallable)
	virtual uint8 GetPriority()
	{
		return Priority;
	}

	UFUNCTION(BlueprintCallable)
	virtual void SetPriority(uint8 InPriority)
	{
		Priority = InPriority;
	}

	UFUNCTION()
	virtual void SetWeight(float InWeight)
	{
		Weight = InWeight;
	}

protected:

	// Instance화된 Axis들
	UPROPERTY()
	TArray<TObjectPtr<UBattleUtilityAxis>> AxisArray;

	UPROPERTY()
	TArray<EAxisType> AxisTypeArray;

	UPROPERTY()
	TArray<TObjectPtr<UBattleUtilityArrayAxis>> ArrayAxisArray;

	UPROPERTY(EditAnywhere)
	uint8 Priority = 2;

	float Weight;

	UPROPERTY()
	TObjectPtr<UBattleUtilityAIComponent> CachedAIComponent;

	UPROPERTY()
	TArray<TObjectPtr<AActor>> BestTargets;

	float ScoreMultiplier = 1.0f;
	
};
