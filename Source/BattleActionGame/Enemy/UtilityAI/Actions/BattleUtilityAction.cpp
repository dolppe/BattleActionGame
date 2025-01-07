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
		else
		{
			UBattleUtilityArrayAxis* NewArrayAxis = nullptr;
			NewArrayAxis = NewObject<UBattleUtilityArrayAxis>(this, UBattleUtilityArrayAxis::StaticClass());

			NewArrayAxis->SetConsiderFactor(AxisConfig.ConsiderType);
			NewArrayAxis->SetFunction(AxisConfig.FunctionType, AxisConfig.Slope, AxisConfig.Exponent, AxisConfig.VerticalShift, AxisConfig.HorizontalShift);
			NewArrayAxis->SetAIComponent(UtilityAIComponent);
			NewArrayAxis->SetAxisType(AxisConfig.AxisType);

			AxisTypeArray.Add(AxisConfig.AxisType);
			ArrayAxisArray.Add(NewArrayAxis);
		}
	}

	CachedAIComponent = UtilityAIComponent;
	
}

float UBattleUtilityAction::EvaluateScore(const UConsiderationFactors* ConsiderList)
{
	float Result = 1.0f; 
	for (UBattleUtilityAxis* UtilityAxis : AxisArray)
	{
		float FactorValue = UtilityAxis->GetValue();
		Result = Result * UtilityAxis->CalcValue(FactorValue);
	}

	TArray<bool> CompletedTypes;
	CompletedTypes.Init(false, AxisTypeArray.Num());

	// EAxisType이 Array쪽일 때 처리
	// ex) TargetArray
	// ArrayAxisArray => 각 ArrayAxis가 존재,
	// 단, 해당 ArrayAxis가 같은 타입인 경우, 내부의 ValueArray는 같은 Index를 의미함
	// A ArrayAxis => Target, B ArrayAxis => Target인 경우
	// A ArrayAxis의 ValueArray의 첫번째 Target == B ArrayAxis의 ValueArray의 두번째 Target
	BestTargets.Empty();
	while (CompletedTypes.Contains(false))
	{
		EAxisType CurrentType =EAxisType::None;

		for (int Idx = 0; Idx < AxisTypeArray.Num();Idx++)
		{
			if (!CompletedTypes[Idx])
			{
				// 미완료 Idx에 접근
				CurrentType = AxisTypeArray[Idx];
				break;
			}
		}
		
		if (CurrentType == EAxisType::None)
		{
			break;
		}

		// 해당 타입인 ArrayAxis의 Index들을 다 가져옴
		TArray<int> IndexArray;
		for (int Idx = 0; Idx < AxisTypeArray.Num();Idx++)
		{
			if (CurrentType == AxisTypeArray[Idx])
			{
				CompletedTypes[Idx] = true;
				IndexArray.Add(Idx);
			}
		}

		int Size = ArrayAxisArray[IndexArray[0]]->GetArrayValue().Num();
		
		float BestScore = 0.0f;
		int BestIdx = 0;
		// ArrayAxis의 모든 Target을 순회함
		// AxisIdx => 0 => 첫 번째 Target이라는 것
		// AxisIdx는 Target을 의미하고 IndexArrayIdx는 Axis를 의미함.
		for (int AxisIdx = 0; AxisIdx < Size; AxisIdx++)
		{
			float MultiplyValue = 1.0f;
			for (int IndexArrayIdx = 0; IndexArrayIdx < IndexArray.Num();IndexArrayIdx++)
			{
				// IndexArrayIdx는 같은 Type의 ArrayAxis가 어떤 것들이 있는지 접근
				// 따라서 같은 Target의 Hp를 담당하는 ArrayAxis, 같은 Target의 Distance를 담당하는 ArrayAxis에 각각 접근
				// 정해진 함수 규칙에 따라서 값을 계산하고, 각 값은 곱해서 관리
				float FactorValue = ArrayAxisArray[IndexArray[IndexArrayIdx]]->GetArrayValue()[AxisIdx];
				FactorValue = ArrayAxisArray[IndexArray[IndexArrayIdx]]->CalcValue(FactorValue);
				// 각 Axis의 값들이 곱해지는 것
				MultiplyValue *= FactorValue;
			}
			
			if (BestScore < MultiplyValue)
			{
				BestScore = MultiplyValue;
				BestIdx = AxisIdx;
			}
		}

		Result = BestScore * Result;
		BestTargets.Add(ConsiderList->GetTargetPtr(CurrentType, BestIdx));
	}
	
	return Result * Weight * Age;
}

void UBattleUtilityAction::StartAction()
{
	UE_LOG(LogBattle, Log, TEXT("StartAction: %s"), *GetName());
}

void UBattleUtilityAction::EndAction()
{
	UE_LOG(LogBattle, Log, TEXT("EndAction: %s"), *GetName());
}

bool UBattleUtilityAction::TickAction(float DeltaTime)
{
	return false;
}


