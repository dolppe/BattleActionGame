#include "BattleUtilityAction.h"

#include "BattleActionGame/BattleLogChannels.h"
#include "NavigationSystem.h"
#include "BattleActionGame/Combat/BattleCombatData.h"
#include "BattleActionGame/Enemy/UtilityAI/BattleUtilityAIData.h"
#include "BattleActionGame/Enemy/UtilityAI/BattleUtilityAxis.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleUtilityAction)


TArray<FVector> BattleUtilityActionHelper::FindBestSpotsWithGreedy(UNavigationSystemV1* NavSystem,
	TArray<FVector> SpotLists, float AreaRadius, int AreaNum)
{
	if (NavSystem == nullptr || AreaNum == 0 || SpotLists.IsEmpty())
	{
		return TArray<FVector>();
	}
	TArray<FVector> Locations;
	TArray<FVector> Result;
	
	for (FVector& Spot : SpotLists)
	{
		Spot.Z = 0.0f;
		Locations.Add(Spot);
	}
	
	if (Locations.Num() <2)
	{
		for (int Idx =0;Idx<Locations.Num();Idx++)
		{
			if (Idx >= AreaNum)
			{
				break;
			}
			FVector AreaData = Locations[Idx];
			FNavLocation ProjectedNavLocation;	
			if (NavSystem->ProjectPointToNavigation(AreaData, ProjectedNavLocation, FVector(300.f,300.f, 6000.f)))
			{
				AreaData = ProjectedNavLocation.Location;
			}
		
			Result.Add(AreaData);
		}
		
		return Result;
	}
	
	TArray<TPair<FVector,FVector>> SelectedLocations;
	
	for (int i = 0; i<Locations.Num();i++)
	{
		FVector& StartLocation = Locations[i];
		for (int j =i+1; j <Locations.Num();j++)
		{
			FVector& EndLocation = Locations[j];
			
			float Distance = FVector::Dist(StartLocation, EndLocation);

			if (Distance <= AreaRadius * 2)
			{
				SelectedLocations.Add({StartLocation, EndLocation});
			}
		}
	}

	if (SelectedLocations.IsEmpty())
	{
		for (int Idx =0;Idx<Locations.Num();Idx++)
		{
			if (Idx >= AreaNum)
			{
				break;
			}
			FVector AreaData = Locations[Idx];
			FNavLocation ProjectedNavLocation;	
		
			if (NavSystem->ProjectPointToNavigation(AreaData, ProjectedNavLocation, FVector(300.f,300.f, 6000.f)))
			{
				AreaData = ProjectedNavLocation.Location;
			}
		
			Result.Add(AreaData);
		}
		
		return Result;
	}

	FAttackData AttackData;
	
	// 선택된 점 세트마다 원 2개 만들기
	TSet<FVector> Centers;

	for (TPair<FVector, FVector>& LocationPair : SelectedLocations)
	{
		FVector& StartVector = LocationPair.Key;
		FVector& EndVector = LocationPair.Value;

		FVector MidVector = (StartVector + EndVector) * 0.5f;

		float HalfDistance = (StartVector - EndVector).Size2D() * 0.5f;

		float MidToCenterDistance = FMath::Sqrt((AreaRadius * AreaRadius) - (HalfDistance * HalfDistance));

		FVector MidUnit = (MidVector - StartVector).GetSafeNormal2D();

		float CenterX1 = MidVector.X + MidToCenterDistance*MidUnit.Y;
		float CenterY1 = MidVector.Y - MidToCenterDistance*MidUnit.X;
		
		float CenterX2 = MidVector.X - MidToCenterDistance*MidUnit.Y;
		float CenterY2 = MidVector.Y + MidToCenterDistance*MidUnit.X;

		FVector NewCenter = FVector(CenterX1, CenterY1, 0.0f);
		FVector NewCenter2 = FVector(CenterX2, CenterY2, 0.0f);

		Centers.Add(NewCenter);
		Centers.Add(NewCenter2);
	}
	
	// 원에 포함되는 점 개수 구하기
	

	const float AreaRadiusSquared = AreaRadius * AreaRadius;

	TArray<TPair<int, FVector>> CalcLocations; 
	int MaxCount = 0;
	
	for (FVector& Center : Centers)
	{
		int Count = 0;
		for (FVector Location : Locations)
		{
			if (FMath::Abs(FVector::DistSquared2D(Location, Center) - AreaRadiusSquared) <= 0.0001)
			{
				Count++;
			}
		}

		CalcLocations.Add({Count, Center});
	}

	CalcLocations.Sort([](const TPair<int, FVector>& A, const TPair<int, FVector>& B)
	{
		return A.Key > B.Key;
	});


	
	for (int Idx =0;Idx<CalcLocations.Num();Idx++)
	{
		if (Idx >= AreaNum)
		{
			break;
		}
		FVector AreaData = CalcLocations[Idx].Value;
		FNavLocation ProjectedNavLocation;	
		
		if (NavSystem->ProjectPointToNavigation(AreaData, ProjectedNavLocation, FVector(300.f,300.f, 6000.f)))
		{
			AreaData = ProjectedNavLocation.Location;
		}
		
		Result.Add(AreaData);
	}
	
	if (CalcLocations.Num() < AreaNum)
	{
		for (int Idx =0;Idx<Locations.Num();Idx++)
		{
			if (Idx + CalcLocations.Num() >= AreaNum)
			{
				break;
			}
			FVector AreaData = Locations[Idx];
			FNavLocation ProjectedNavLocation;	
		
			if (NavSystem->ProjectPointToNavigation(AreaData, ProjectedNavLocation, FVector(300.f,300.f, 6000.f)))
			{
				AreaData = ProjectedNavLocation.Location;
			}
		
			Result.Add(AreaData);
		}
	}
	

	return Result;
	
}

UBattleUtilityAction::UBattleUtilityAction()
{
}

void UBattleUtilityAction::InitAction(FActionConfig& ActionConfig, UBattleUtilityAIComponent* UtilityAIComponent)
{
	Weight = ActionConfig.Weight;
	CoolTime = ActionConfig.CoolTime;
	AgeTime = ActionConfig.AgeTime;
	AgeRate = ActionConfig.AgeRate;
	DistanceSourceType = ActionConfig.DistanceSourceType;
	MinDistance = ActionConfig.MinDistance;
	MaxDistance = ActionConfig.MaxDistance;
	
	
	for (FAxisConfig AxisConfig : ActionConfig.AxisConfigs)
	{
		if (AxisConfig.AxisType == EAxisType::Single)
		{
			UBattleUtilityAxis* NewAxis = nullptr;
			NewAxis = NewObject<UBattleUtilityAxis>(this, UBattleUtilityAxis::StaticClass());

			NewAxis->SetConsiderFactor(AxisConfig.ConsiderType);
			NewAxis->SetFunction(AxisConfig.FunctionType, AxisConfig.Slope, AxisConfig.Exponent, AxisConfig.VerticalShift, AxisConfig.HorizontalShift, AxisConfig.AxisMinValue, AxisConfig.AxisMaxValue);
			NewAxis->SetAIComponent(UtilityAIComponent);

			AxisArray.Add(NewAxis);
		}
		else
		{
			UBattleUtilityArrayAxis* NewArrayAxis = nullptr;
			NewArrayAxis = NewObject<UBattleUtilityArrayAxis>(this, UBattleUtilityArrayAxis::StaticClass());

			NewArrayAxis->SetConsiderFactor(AxisConfig.ConsiderType);
			NewArrayAxis->SetFunction(AxisConfig.FunctionType, AxisConfig.Slope, AxisConfig.Exponent, AxisConfig.VerticalShift, AxisConfig.HorizontalShift, AxisConfig.AxisMinValue, AxisConfig.AxisMaxValue);
			NewArrayAxis->SetAIComponent(UtilityAIComponent);
			NewArrayAxis->SetAxisType(AxisConfig.AxisType);

			AxisTypeArray.Add(AxisConfig.AxisType);
			ArrayAxisArray.Add(NewArrayAxis);
		}
	}

	CachedAIComponent = UtilityAIComponent;
	
}

PRAGMA_DISABLE_OPTIMIZATION

float UBattleUtilityAction::EvaluateScore(const UConsiderationFactors* ConsiderList)
{
	
	TArray<FUtilityAIScoreData>& UtilityAIScoreDatas = CachedAIComponent->GetUtilityAIScoreData();
		
	float Result = 1.0f; 
	for (UBattleUtilityAxis* UtilityAxis : AxisArray)
	{
		float FactorValue = UtilityAxis->GetValue();
		float CalcValue = UtilityAxis->CalcValue(FactorValue);

		if (!UtilityAIScoreDatas.IsEmpty())
		{
			UtilityAIScoreDatas.Last().NormalFactorScoreOrigin.Add(FactorValue);
			UtilityAIScoreDatas.Last().NormalFactorScoreFinal.Add(CalcValue);
			UtilityAIScoreDatas.Last().NormalFactorConsiderType.Add(UtilityAxis->GetConsiderFactor());	
		}
		
		
		Result = Result * CalcValue;
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
				CompletedTypes[Idx] = true;
				break;
			}
		}
		
		if (CurrentType == EAxisType::None)
		{
			break;
		}

		// 해당 타입인 ArrayAxis의 Index들을 다 가져옴
		TArray<int> IndexArray;
		for (int Idx = 0; Idx < ArrayAxisArray.Num();Idx++)
		{
			if (CurrentType == ArrayAxisArray[Idx]->GetAxisType())
			{
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
			// 하나의 Target에 대한 최종값
			float MultiplyValue = 1.0f;
			FArrayFactorData ArrayFactorData;
			
			for (int IndexArrayIdx = 0; IndexArrayIdx < IndexArray.Num();IndexArrayIdx++)
			{
				// IndexArrayIdx는 같은 Type의 ArrayAxis가 어떤 것들이 있는지 접근
				// 따라서 같은 Target의 Hp를 담당하는 ArrayAxis, 같은 Target의 Distance를 담당하는 ArrayAxis에 각각 접근
				// 정해진 함수 규칙에 따라서 값을 계산하고, 각 값은 곱해서 관리
				// Target(AxisIdx)에 대해서 Axis(IndexArrayIdx) 하나의 원본 값
				
				//ArrayFactorData.AxisType = ArrayAxisArray[IndexArray[IndexArrayIdx]]->GetConsiderFactor();
				ArrayFactorData.AxisType = CurrentType;
				float FactorValue = ArrayAxisArray[IndexArray[IndexArrayIdx]]->GetArrayValue()[AxisIdx];
				ArrayFactorData.ArrayFactorScoreOrigin.Add(FactorValue);
				// Target(AxisIdx)에 대해서 Axis(IndexArrayIdx) 하나의 계산된 값
				float CalcValue = ArrayAxisArray[IndexArray[IndexArrayIdx]]->CalcValue(FactorValue);
				ArrayFactorData.ArrayFactorScoreFinal.Add(CalcValue);
				// 각 Axis의 값들이 곱해지는 것
				MultiplyValue *= CalcValue;
			}
			if (!UtilityAIScoreDatas.IsEmpty())
			{
				UtilityAIScoreDatas.Last().ArrayFactorScore.Add(ArrayFactorData);	
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
	
	return Result * Weight;
}

PRAGMA_ENABLE_OPTIMIZATION

void UBattleUtilityAction::StartAction()
{
	//UE_LOG(LogBattle, Log, TEXT("StartAction: %s"), *GetName());
}

void UBattleUtilityAction::EndAction()
{
	//UE_LOG(LogBattle, Log, TEXT("EndAction: %s"), *GetName());
	bIsCompletedAction = true;
}

void UBattleUtilityAction::TickAction(float DeltaTime)
{
	
}

bool UBattleUtilityAction::CanAllowedRange(float InDistance)
{
	if (MinDistance <= InDistance && InDistance <= MaxDistance)
	{
		return true;
	}
	
	return false;
}

void UBattleUtilityAction::SetStartAction()
{
	bIsCompletedAction = false;
}


