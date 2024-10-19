#include "BattleUtilityAction.h"

#include "BattleActionGame/BattleLogChannels.h"
#include "BattleActionGame/Enemy/UtilityAI/BattleUtilityAIData.h"
#include "BattleActionGame/Enemy/UtilityAI/BattleUtilityAxis.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleUtilityAction)


UBattleUtilityAction::UBattleUtilityAction()
{
}

void UBattleUtilityAction::InitAxis(TArray<FAxisConfig> AxisConfigs, UBattleUtilityAIComponent* UtilityAIComponent)
{
	for (FAxisConfig AxisConfig : AxisConfigs)
	{
		if (AxisConfig.AxisType == EAxisType::Single)
		{
			UBattleUtilityAxis* NewAxis = nullptr;
			NewAxis = NewObject<UBattleUtilityAxis>(this, UBattleUtilityAxis::StaticClass());

			NewAxis->SetConsiderFactor(AxisConfig.ConsiderType);
			NewAxis->SetFunction(AxisConfig.FunctionType, AxisConfig.Slope, AxisConfig.Exponent, AxisConfig.VerticalShift, AxisConfig.HorizontalShift);
			NewAxis->SetAIComponent(UtilityAIComponent);

			AxisArray.Add(NewAxis);
		}
		else if (AxisConfig.AxisType == EAxisType::Array)
		{
			UBattleUtilityArrayAxis* NewArrayAxis = nullptr;
			NewArrayAxis = NewObject<UBattleUtilityArrayAxis>(this, UBattleUtilityArrayAxis::StaticClass());

			NewArrayAxis->SetConsiderFactor(AxisConfig.ConsiderType);
			NewArrayAxis->SetFunction(AxisConfig.FunctionType, AxisConfig.Slope, AxisConfig.Exponent, AxisConfig.VerticalShift, AxisConfig.HorizontalShift);
			NewArrayAxis->SetAIComponent(UtilityAIComponent);

			ArrayAxisArray.Add(NewArrayAxis);
		}
	}

	CachedAIComponent = UtilityAIComponent;
	
}

float UBattleUtilityAction::EvaluateScore(const UConsiderationFactors* ConsiderList)
{
	UE_LOG(LogBattle, Log, TEXT("EvaluateScoreStart"));
	float Result = 1.0f; 
	for (UBattleUtilityAxis* UtilityAxis : AxisArray)
	{
		float FactorValue = UtilityAxis->GetValue();
		Result = Result * UtilityAxis->CalcValue(FactorValue);
		UE_LOG(LogBattle, Log, TEXT("%s Axis Result: %f"), *EnumToString(UtilityAxis->GetConsiderFactor()),Result);
	}
	
	
	for (UBattleUtilityArrayAxis* UtilityArrayAxis : ArrayAxisArray)
	{
		TArray<float> ArrayFactorValue = UtilityArrayAxis->GetArrayValue();
		float BestScore = 0.0f;
		for (float FactorValue : ArrayFactorValue)
		{
			float CurScore = UtilityArrayAxis->CalcValue(FactorValue);
			if (BestScore < CurScore)
			{
				BestScore = CurScore;
				UE_LOG(LogBattle, Log, TEXT("Best Score: %f"), BestScore);
			}
		}
		Result = BestScore * Result;
		UE_LOG(LogBattle, Log, TEXT("%s ArrayAxis Result: %f"), *EnumToString(UtilityArrayAxis->GetConsiderFactor()),Result);
	}

	UE_LOG(LogBattle, Log, TEXT("Weight: %f"), Weight);
	return Result * Weight;
}

void UBattleUtilityAction::StartAction()
{
	UE_LOG(LogBattle, Log, TEXT("Start Action: %s"), *this->GetName());
}

void UBattleUtilityAction::EndAction()
{
}

bool UBattleUtilityAction::TickAction(float DeltaTime)
{
	return false;
}


