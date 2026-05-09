#pragma once

#include "BattleActionGame/Enemy/UtilityAI/BattleUtilityAIComponent.h"
#include "BattleUtilityAction.generated.h"

enum class EDistanceSourceType : uint8;
struct FActionConfig;
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
	void InitAction(FActionConfig& ActionConfig, UBattleUtilityAIComponent* UtilityAIComponent);
	
	UFUNCTION()
	virtual float EvaluateScore(const UConsiderationFactors* ConsiderList);

	UFUNCTION()
	virtual void StartAction();

	UFUNCTION()
	virtual void EndAction();

	UFUNCTION()
	virtual void TickAction(float DeltaTime);

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
	
	float GetCoolTime() const
	{
		return CoolTime;
	}
	
	float GetAgeTime() const
	{
		return AgeTime;
	}
	
	float GetAgeRate() const
	{
		return AgeRate;
	}
	
	bool CanAllowedRange(float InDistance);
	EDistanceSourceType GetDistanceSourceType() const
	{
		return DistanceSourceType;
	}
	
	bool IsCompleteAction() const
	{
		return bIsCompletedAction;
	}
	
	void SetStartAction();


	
	

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

	UPROPERTY()
	TObjectPtr<UBattleUtilityAIComponent> CachedAIComponent;

	UPROPERTY()
	TArray<TObjectPtr<AActor>> BestTargets;
	
	float Weight;
	
	float CoolTime = 0.0f;
	float AgeTime = 0.0f;
	float AgeRate = 1.0f;
	
	EDistanceSourceType DistanceSourceType;
	float MinDistance;
	float MaxDistance;
	
	bool bIsCompletedAction = false;
	

	
};
