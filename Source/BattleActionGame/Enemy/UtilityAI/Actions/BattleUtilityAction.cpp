#include "BattleUtilityAction.h"

#include "BattleActionGame/Enemy/UtilityAI/BattleUtilityAIData.h"
#include "BattleActionGame/Enemy/UtilityAI/Axis/BattleUtilityAxis.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleUtilityAction)

UBattleUtilityAction::UBattleUtilityAction()
{
}

void UBattleUtilityAction::InitAxis(TArray<FAxisConfig> AxisConfigs)
{
	for (FAxisConfig AxisConfig : AxisConfigs)
	{
		UBattleUtilityAxis* NewAxis = NewObject<UBattleUtilityAxis>(this, AxisConfig.AxisClass);

		NewAxis->SetWeight(AxisConfig.DefaultWeight);
		// Math쪽에서 Enum으로 Function을 넣어주기
		NewAxis->SetCurveData(AxisConfig.CurveMapping);

		AxisArray.Add(NewAxis);
	}
	
}

float UBattleUtilityAction::EvaluateScore(const FConsiderationFactors& ConsiderList)
{
	float Result = 1.0f; 
	for (UBattleUtilityAxis* UtilityAxis : AxisArray)
	{
		float FactorValue = ConsiderList.Factors[UtilityAxis->GetConsiderFactor()];
		Result = Result * UtilityAxis->CalcValue(FactorValue);
	}

	return Result;
}

void UBattleUtilityAction::StartAction()
{
}

void UBattleUtilityAction::EndAction()
{
}

bool UBattleUtilityAction::TickAction(float DeltaTime)
{
	return false;
}


