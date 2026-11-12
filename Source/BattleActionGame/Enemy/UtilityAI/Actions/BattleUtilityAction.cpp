#include "BattleUtilityAction.h"

#include "BattleActionGame/Enemy/UtilityAI/BattleConsiderationFactors.h"
#include "BattleActionGame/Enemy/UtilityAI/BattleUtilityAIData.h"
#include "BattleActionGame/Enemy/UtilityAI/BattleUtilityAxis.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleUtilityAction)


TArray<FVector> BattleUtilityActionHelper::FindBestSpotsWithGreedy(const TArray<FVector>& SpotLists, float AreaRadius, int32 AreaNum)
{
	if (AreaNum <= 0 || AreaRadius <= 0 || SpotLists.IsEmpty())
	{
		return TArray<FVector>();
	}
	
	// SpotList에서 X와 Y만 사용함
	TArray<FVector> Locations;
	TArray<bool> IsLocationsSelected;
	TArray<FVector> Result;
	
	Locations.Reserve(SpotLists.Num());
	IsLocationsSelected.Reserve(SpotLists.Num());
	Result.Reserve(AreaNum);
	
	for (const FVector& Spot : SpotLists)
	{
		Locations.Add(FVector(Spot.X, Spot.Y, 0.0f));
		IsLocationsSelected.Add(false);
	}
	
	// 두 점 사이의 거리가 원의 지름 이하인 점 쌍을 저장
	TArray<TPair<FVector,FVector>> ValidPointPairs;
	
	for (int32 i = 0; i<Locations.Num();i++)
	{
		const FVector& StartLocation = Locations[i];
		for (int32 j =i+1; j <Locations.Num();j++)
		{
			const FVector& EndLocation = Locations[j];
			
			float Distance = FVector::Dist(StartLocation, EndLocation);

			if (Distance <= AreaRadius * 2)
			{
				ValidPointPairs.Add({StartLocation, EndLocation});
			}
		}
	}
	
	// 원의 중심 후보 저장
	TSet<FVector> CandidateCenters;
	const float AreaRadiusSquared = AreaRadius * AreaRadius;
	
	for (const FVector& Location : Locations)
	{
		CandidateCenters.Add(Location);
	}

	// ValidPointPairs: 두 지점 사이의 거리가 지름 이하인 점쌍
	if (!ValidPointPairs.IsEmpty())
	{
		for (const TPair<FVector, FVector>& PointPair : ValidPointPairs)
		{
			const FVector& PointA = PointPair.Key;
			const FVector& PointB = PointPair.Value;

			const FVector MidPoint = (PointA + PointB) * 0.5f;
			const float HalfDistance = FVector::Dist2D(PointA, PointB) * 0.5f;
			
			// 중점에서 원의 중심까지의 거리 계산
			const float CenterOffsetDistance = FMath::Sqrt(
				FMath::Max(
					(AreaRadiusSquared) - (HalfDistance * HalfDistance),0.0f));

			// PointA에서 Mid로 향하는 단위 방향 벡터
			const FVector MidUnit = (MidPoint - PointA).GetSafeNormal2D();
			// Mid에서 Center로 향하는 방향
			const FVector MidToCenter(MidUnit.Y, -MidUnit.X, 0.0f);
			const FVector CenterOffset = MidToCenter * CenterOffsetDistance;
						
			CandidateCenters.Add(MidPoint + CenterOffset);
			CandidateCenters.Add(MidPoint - CenterOffset);
		}
	}

	
	// 원에 포함되는 점 개수 구하여 최종 원의 중심 확정
	int32 CoveredTargetCount = 0;
	int32 UniqueCenterCount = 0;
	
	for (int32 AreaIdx =0; AreaIdx < AreaNum; ++AreaIdx)
	{
		// 모든 점이 한번씩은 커버됐다면 기존에 구했던 것 재사용
		if (CoveredTargetCount >= Locations.Num())
		{
			FVector CurLocation = Result[AreaIdx % UniqueCenterCount];
			Result.Add(CurLocation);
			continue;
		}
		
		FVector BestCenter = FVector::ZeroVector;
		TArray<int32> BestCoveredIndexes;
		
		for (FVector& Center : CandidateCenters)
		{
			TArray<int32> CoveredIndexes;
			
			// 아직 선택되지 않은 대상 중 현재 후보의 범위 안에 있는 대상 수집
			for (int32 LocationIdx = 0; LocationIdx < Locations.Num(); ++LocationIdx)
			{
				if (!IsLocationsSelected[LocationIdx] && 
					FVector::DistSquared2D(Locations[LocationIdx], Center) <= AreaRadiusSquared)
				{
					CoveredIndexes.Add(LocationIdx);
				}
			}
			
			// 미선택 대상을 가장 많이 포함하는 중심과 대상 목록 저장
			if (BestCoveredIndexes.Num() < CoveredIndexes.Num())
			{
				BestCoveredIndexes = MoveTemp(CoveredIndexes);
				BestCenter = Center;
			}
		}
		
		// 포함된 대상은 다음 공격 범위 선택 시 평가에서 제외
		for (int32 SelectedLocationIdx : BestCoveredIndexes)
		{
			IsLocationsSelected[SelectedLocationIdx] = true;
		}
		
		Result.Add(BestCenter);
		
		CoveredTargetCount += BestCoveredIndexes.Num();
		UniqueCenterCount++;
	}
	
	return Result;
}

UBattleUtilityAction::UBattleUtilityAction()
{
}

void UBattleUtilityAction::InitAction(const FActionConfig& ActionConfig, UConsiderationFactors* InConsiderationFactors)
{
	// Action 기본 세팅값 설정
	Weight = ActionConfig.Weight;
	CoolTime = ActionConfig.CoolTime;
	AgeTime = ActionConfig.AgeTime;
	AgeRate = ActionConfig.AgeRate;
	DistanceSourceType = ActionConfig.DistanceSourceType;
	MinDistance = ActionConfig.MinDistance;
	MaxDistance = ActionConfig.MaxDistance;
	
	ConsiderationFactors = InConsiderationFactors;
	
	// Axis 생성 및 초기화
	for (const FAxisConfig& AxisConfig : ActionConfig.AxisConfigs)
	{
		UBattleUtilityAxis* NewAxis = NewObject<UBattleUtilityAxis>(this, UBattleUtilityAxis::StaticClass());
		NewAxis->SetConsiderType(AxisConfig.ConsiderType);
		NewAxis->SetFunction(AxisConfig);
		
		EAxisType AxisType = UConsiderationFactors::GetAxisType(AxisConfig.ConsiderType);
		if (AxisType == EAxisType::Single)
		{
			SingleAxisList.Add(NewAxis);
		}
		else
		{
			AxisTypes.Add(AxisType);
			MultiAxisList.Add(NewAxis);
		}
	}	
}

float UBattleUtilityAction::EvaluateScore()
{
	BestTargets.Reset();
	
	UConsiderationFactors* ConsiderList = ConsiderationFactors.Get();
	
	if (!IsValid(ConsiderList))
	{
		return 0.0f;
	}
	
	// 단일 Axis 점수들을 곱함
	float ActionScore = Weight; 
	for (UBattleUtilityAxis* SingleAxis : SingleAxisList)
	{
		// 미리 계산한 평가값 조회
		const float FactorValue = ConsiderList->GetFactorValue(SingleAxis->GetConsiderType());
		
		// 해당 Axis에 설정된 함수로 Axis 점수 계산
		const float AxisScore = SingleAxis->CalcValue(FactorValue);
		
		ActionScore *= AxisScore;
	}
	
	for (EAxisType AxisType : AxisTypes)
	{
		// 같은 AxisType에 대한 MultiAxis들을 수집
		TArray<UBattleUtilityAxis*> MatchingAxisList;
		for (int32 Idx = 0; Idx < MultiAxisList.Num();Idx++)
		{
			if (AxisType == UConsiderationFactors::GetAxisType(MultiAxisList[Idx]->GetConsiderType()))
			{
				MatchingAxisList.Add(MultiAxisList[Idx]);
			}
		}
		
		// 현재 타입에 해당하는 MultiAxis가 반드시 존재해야 함
		check(!MatchingAxisList.IsEmpty());
		
		// MatchingAxisList: 같은 대상 유형을 평가하는 MultiAxis 목록
		// MultiAxis에서 같은 인덱스는 동일한 후보를 의미
		const TArray<float>* FirstFactors = ConsiderList->GetMultiFactorValues(MatchingAxisList[0]->GetConsiderType());
		if (!FirstFactors || FirstFactors->IsEmpty())
		{
			return 0.0f;
		}
			
		const int32 TargetCount = FirstFactors->Num();
		
		float BestTargetScore = 0.0f;
		int32 BestTargetIndex = 0;
		
		for (int32 TargetIndex = 0; TargetIndex < TargetCount; ++TargetIndex)
		{
			AActor* Candidate = ConsiderList->GetCandidatePtr(AxisType, TargetIndex);
			if (!IsValid(Candidate))
			{
				continue;
			}
			// 후보별로 모든 MultiAxis 점수를 곱하여 결합
			float TargetScore = 1.0f;
			for (UBattleUtilityAxis* MultiAxis : MatchingAxisList)
			{
				const TArray<float>* MultiFactors = ConsiderList->GetMultiFactorValues(MultiAxis->GetConsiderType());
				
				// 후보의 Axis들 중 하나라도 이상하면 아예 해당 후보는 빼버림
				if (!MultiFactors || !MultiFactors->IsValidIndex(TargetIndex))
				{
					TargetScore = 0.0f;
					break;
				}
				
				const float FactorValue = (*MultiFactors)[TargetIndex];
				const float AxisScore = MultiAxis->CalcValue(FactorValue);
				
				TargetScore *= AxisScore;
			}
			
			// 최고 점수와 해당 후보의 인덱스 저장
			if (BestTargetScore < TargetScore)
			{
				BestTargetScore = TargetScore;
				BestTargetIndex = TargetIndex;
			}
		}
		
		// 최적 대상 점수를 행동 점수에 반영하고, 실행에 사용할 대상 저장
		ActionScore *= BestTargetScore;
		BestTargets.Add(AxisType, ConsiderList->GetCandidatePtr(AxisType, BestTargetIndex));
	}
	
	return ActionScore;
}

void UBattleUtilityAction::StartAction()
{
}

void UBattleUtilityAction::EndAction()
{
	bIsCompletedAction = true;
}

void UBattleUtilityAction::TickAction(float DeltaTime)
{
}

bool UBattleUtilityAction::CanAllowedRange(float InDistance) const
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


