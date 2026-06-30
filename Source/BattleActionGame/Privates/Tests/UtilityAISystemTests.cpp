
#if WITH_DEV_AUTOMATION_TESTS

#include "UtilityAISystemTests.h"

#include "BattleActorTestFixture.h"
#include "BattleCharacterTests.h"
#include "NavigationSystem.h"
#include "BattleActionGame/BattleGameplayTags.h"
#include "BattleActionGame/Character/BattleCharacterBase.h"
#include "BattleActionGame/Enemy/BattleEnemyCharacter.h"
#include "BattleActionGame/Enemy/UtilityAI/BattleUtilityAIComponent.h"
#include "BattleActionGame/Enemy/UtilityAI/BattleUtilityAIData.h"
#include "BattleActionGame/Enemy/UtilityAI/Actions/BattleUtilityAction_Search.h"
#include "BattleActionGame/Math/CalcFunctions.h"
#include "BattleActionGame/Navigation/NavArea/BattleNavArea.h"

IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(FConsiderationFactorsTest, FBattleActorTestFixture,"BattleAction.UtilityAI.ConsiderList", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter);
IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(FUtilityAIComponentTest, FBattleActorTestFixture,"BattleAction.UtilityAI.UtilityAIComponent", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter);
IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(FUtilityActionTest, FBattleActorTestFixture,"BattleAction.UtilityAI.UtilityAction", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter);
IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(FUtilityActionSubClassTest, FBattleActorTestFixture,"BattleAction.UtilityAI.UtilityActionSubClass", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter);
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCalcFunctionTest, "BattleAction.Utility.CalcFunction", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter);
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FUtilityAIDataTest, "BattleAction.UtilityAI.UtilityAIData", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter);

bool FUtilityAIDataTest::RunTest(const FString& Parameters)
{
	FAxisConfig TestAxisConfig;
	
	TestAxisConfig.FunctionPreset = EAxisFunctionPreset::EaseOutHigh;
	UBattleUtilityAIData::ApplyAxisFunctionPreset(TestAxisConfig);
	
	TestEqual(TEXT("설정된 FunctionType으로 변경되어야 함"), TestAxisConfig.FunctionType, EAxisFunction::Quadratic);
	TestEqual(TEXT("설정된 Slope값으로 변경되어야 함"), TestAxisConfig.Slope, -1.0f);
	TestEqual(TEXT("설정된 Exponent값으로 변경되어야 함"), TestAxisConfig.Exponent, 2.0f);
	TestEqual(TEXT("설정된 VerticalShift값으로 변경되어야 함"), TestAxisConfig.VerticalShift, 1.0f);
	TestEqual(TEXT("설정된 HorizontalShift값으로 변경되어야 함"), TestAxisConfig.HorizontalShift, 1.0f);
	
	TestAxisConfig.FunctionPreset = EAxisFunctionPreset::Custom;
	TestAxisConfig.FunctionType = EAxisFunction::Logistic;
	TestAxisConfig.Slope = -100.f;
	TestAxisConfig.Exponent = 200.f;
	TestAxisConfig.VerticalShift = -200.f;
	TestAxisConfig.HorizontalShift = 100.f;
	
	UBattleUtilityAIData::ApplyAxisFunctionPreset(TestAxisConfig);
	
	TestEqual(TEXT("Custom은 아무것도 변경하면 안됨"), TestAxisConfig.FunctionType, EAxisFunction::Logistic);
	TestEqual(TEXT("Custom은 아무것도 변경하면 안됨"), TestAxisConfig.Slope, -100.0f);
	TestEqual(TEXT("Custom은 아무것도 변경하면 안됨"), TestAxisConfig.Exponent, 200.0f);
	TestEqual(TEXT("Custom은 아무것도 변경하면 안됨"), TestAxisConfig.VerticalShift, -200.0f);
	TestEqual(TEXT("Custom은 아무것도 변경하면 안됨"), TestAxisConfig.HorizontalShift, 100.0f);
	
	TestAxisConfig.FunctionPreset = EAxisFunctionPreset::LogitEndLowStrong;
	UBattleUtilityAIData::ApplyAxisFunctionPreset(TestAxisConfig);
	
	TestEqual(TEXT("설정된 FunctionType으로 변경되어야 함"), TestAxisConfig.FunctionType, EAxisFunction::Logit);
	TestEqual(TEXT("설정된 Slope값으로 변경되어야 함"), TestAxisConfig.Slope, -0.18f);
	TestEqual(TEXT("설정된 Exponent값으로 변경되어야 함"), TestAxisConfig.Exponent, 1.0f);
	TestEqual(TEXT("설정된 VerticalShift값으로 변경되어야 함"), TestAxisConfig.VerticalShift, 0.5f);
	TestEqual(TEXT("설정된 HorizontalShift값으로 변경되어야 함"), TestAxisConfig.HorizontalShift, 0.0f);
	
	return true;
}

bool FCalcFunctionTest::RunTest(const FString& Parameters)
{
	float X, Slope, Exponent, Vertical, Horizontal, MinValue, MaxValue;
	float WantedValue;
	
	Slope = 1.f;
	Exponent = 1.f;
	Vertical = 0.0f;
	Horizontal = 0.0f;
	MinValue = 0.0f;
	MaxValue = 1.0f;
	
	X = 0.5f;
	WantedValue = 0.5f;
	
	TestEqual(TEXT("LinearFunction은 정확한 값을 리턴 해야 함"), UCalcFunctions::LinearFunction(X,Slope, Exponent, Vertical, Horizontal, MinValue, MaxValue), WantedValue);
	
	Slope = -1.f;
	Exponent = 2.f;
	Vertical = 1.0f;
	Horizontal = 1.0f;
	
	X = 0.5f;
	WantedValue = 0.75f;
	
	TestEqual(TEXT("Quadratic은 정확한 값을 리턴 해야 함"), UCalcFunctions::QuadraticFunction(X,Slope, Exponent, Vertical, Horizontal, MinValue, MaxValue), WantedValue);
	
	Slope = 16.f;
	Exponent = 1.f;
	Vertical = 0.0f;
	Horizontal = 0.5f;
	
	X = 0.5f;
	WantedValue = 0.5f;
	
	TestEqual(TEXT("Logistic은 정확한 값을 리턴 해야 함"), UCalcFunctions::LogisticFunction(X,Slope, Exponent, Vertical, Horizontal, MinValue, MaxValue), WantedValue);
	X = 0.6f;
	WantedValue = 0.832018f;
	
	TestEqual(TEXT("Logistic은 정확한 값을 리턴 해야 함"), UCalcFunctions::LogisticFunction(X,Slope, Exponent, Vertical, Horizontal, MinValue, MaxValue), WantedValue);
	
	Vertical = 0.5f;
	X = 0.5f;
	WantedValue = 1.0f;
	
	TestEqual(TEXT("MaxValue를 넘어서면 MaxValue의 값을 리턴 해야함"), UCalcFunctions::LogisticFunction(X,Slope, Exponent, Vertical, Horizontal, MinValue, MaxValue), WantedValue);
	
	Slope = -0.18f;
	Exponent = 1.f;
	Vertical = 0.5f;
	Horizontal = 0.0f;
	
	X = 0.5f;
	WantedValue = 0.5f;
	
	TestEqual(TEXT("Logit은 정확한 값을 리턴 해야 함"), UCalcFunctions::LogitFunction(X,Slope, Exponent, Vertical, Horizontal, MinValue, MaxValue), WantedValue);
	
	X = 0.2f;
	WantedValue = 0.749533f;
	
	TestEqual(TEXT("Logit은 정확한 값을 리턴 해야 함"), UCalcFunctions::LogitFunction(X,Slope, Exponent, Vertical, Horizontal, MinValue, MaxValue), WantedValue);
	
	X = 0.95f;
	WantedValue = 0.0f;
	
	TestEqual(TEXT("MinValue보다 작아지면 MinValue의 값을 리턴 해야함"), UCalcFunctions::LogitFunction(X,Slope, Exponent, Vertical, Horizontal, MinValue, MaxValue), WantedValue);
	
	
	return true;
}


bool FUtilityActionSubClassTest::RunTest(const FString& Parameters)
{
	UWorld::InitializationValues NewIVS = UWorld::InitializationValues();
	NewIVS.bCreateNavigation = true;
	SetInitializationValues(&NewIVS);
	
	if (!SetupWorld())
	{
		return false;
	}
	
	ON_SCOPE_EXIT{
		ClearWorld();
	};
	
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	
	if (NavSystem == nullptr)
	{
		return false;
	}
	
	TArray<FVector> TestSpotList, ResultArr;
	float TestAreaRadius = 300.f;
	int TestAreaNum = 3;
	
	ResultArr = BattleUtilityActionHelper::FindBestSpotsWithGreedy(NavSystem, TestSpotList, TestAreaRadius, TestAreaNum);
	
	TestEqual(TEXT("전달된 Spot이 하나도 없으면 결과도 비어 있어야 함"), ResultArr.Num(), 0);
	
	FVector Spot_A = FVector(10.f,10.f,0.0f);
	
	TestSpotList.Add(Spot_A);
	
	ResultArr = BattleUtilityActionHelper::FindBestSpotsWithGreedy(NavSystem, TestSpotList, TestAreaRadius, TestAreaNum);
	
	TestEqual(TEXT("하나의 Spot만 전달되면 해당 Spot만 선택 됨"), ResultArr.Num(), 1);
	if (ResultArr.Num() != 1)
	{
		return false;
	}
	TestTrue(TEXT("하나의 Spot만 전달되면 해당 Spot만 선택 됨"), ResultArr[0].Equals(Spot_A));
	
	FVector Spot_B = FVector(100.f, 100.f, 0.0f);
	TestSpotList.Add(Spot_B);
	TestAreaRadius = 10.f;
	
	ResultArr = BattleUtilityActionHelper::FindBestSpotsWithGreedy(NavSystem, TestSpotList, TestAreaRadius, TestAreaNum);
	
	TestEqual(TEXT("두개의 Spot이 전달되고, 두 Spot 사이의 거리가 AreaRadius보다 멀면 그냥 두 스팟만 선택됨"), ResultArr.Num(), 2);
	if (ResultArr.Num() != 2)
	{
		return false;
	}
	TestTrue(TEXT("두개의 Spot이 전달되고, 두 Spot 사이의 거리가 AreaRadius보다 멀면 그냥 두 스팟만 선택됨"), ResultArr[0].Equals(Spot_A));
	TestTrue(TEXT("두개의 Spot이 전달되고, 두 Spot 사이의 거리가 AreaRadius보다 멀면 그냥 두 스팟만 선택됨"), ResultArr[1].Equals(Spot_B));
	
	TestAreaNum = 5;
	FVector Spot_C = FVector(12.f, 12.f,0.0f);
	TestSpotList.Add(Spot_C);
	
	ResultArr = BattleUtilityActionHelper::FindBestSpotsWithGreedy(NavSystem, TestSpotList, TestAreaRadius, TestAreaNum);
	
	TArray<FVector> NewSpots;
	
	int Count = 0;
	
	for (FVector& ResultSpot : ResultArr)
	{
		if (ResultSpot.Equals(Spot_A) || ResultSpot.Equals(Spot_B) || ResultSpot.Equals(Spot_C))
		{
			Count++;
			continue;
		}
		
		NewSpots.Add(ResultSpot);
	}
	TestEqual(TEXT("신규 Spot의 개수보다 AreaNum이 더 많이 남으면 그건 입력으로 받은 Spot으로 채움"), Count, 3);
	TestEqual(TEXT("세 개 중 두개의 Spot만 거리 조건을 만족하면 총 2개의 새로운 Spot을 찾음"), NewSpots.Num(), 2);
	if (NewSpots.Num() != 2)
	{
		return false;
	}
	
	double Distance = FVector::Distance(NewSpots[0], Spot_A);
	TestTrue(TEXT("새로 생긴 Spot은 원래의 위치와 AreaRadius만큼의 거리 차이를 가짐"), FMath::Abs(Distance - TestAreaRadius) <= 0.01);
	Distance = FVector::Distance(NewSpots[0], Spot_C);
	TestTrue(TEXT("새로 생긴 Spot은 원래의 위치와 AreaRadius만큼의 거리 차이를 가짐"), FMath::Abs(Distance - TestAreaRadius) <= 0.01);
	Distance = FVector::Distance(NewSpots[1], Spot_A);
	TestTrue(TEXT("새로 생긴 Spot은 원래의 위치와 AreaRadius만큼의 거리 차이를 가짐"), FMath::Abs(Distance - TestAreaRadius) <= 0.01);
	Distance = FVector::Distance(NewSpots[1], Spot_C);
	TestTrue(TEXT("새로 생긴 Spot은 원래의 위치와 AreaRadius만큼의 거리 차이를 가짐"), FMath::Abs(Distance - TestAreaRadius) <= 0.01);
	
	FVector Spot_D = FVector(15.f, 15.f, 0.0f);
	TestAreaRadius = 20.f;
	TestSpotList.Add(Spot_D);
	ResultArr = BattleUtilityActionHelper::FindBestSpotsWithGreedy(NavSystem, TestSpotList, TestAreaRadius, TestAreaNum);
	
	NewSpots.Empty();
	Count =0;
	for (FVector& ResultSpot : ResultArr)
	{
		if (ResultSpot.Equals(Spot_A) || ResultSpot.Equals(Spot_B) || ResultSpot.Equals(Spot_C) || ResultSpot.Equals(Spot_D))
		{
			Count++;
			continue;
		}
		
		NewSpots.Add(ResultSpot);
	}
	TestEqual(TEXT("새로 생긴 Spot의 개수가 AreaNum보다 많으면 그것들로만 선택함"), Count, 0);
	TestEqual(TEXT("새로 생긴 Spot의 개수가 AreaNum보다 많으면 그것들로만 선택함"), NewSpots.Num(), 5);
	
	return true;
}



bool FUtilityActionTest::RunTest(const FString& Parameters)
{
	if (!SetupWorld())
	{
		return false;
	}
	
	ON_SCOPE_EXIT{
		ClearWorld();
	};
	
	
	UBattleUtilityAIComponent* UtilityAIComponent = NewObject<UBattleUtilityAIComponent>();
	
	UMockTestUtilityAction* UtilityAction = NewObject<UMockTestUtilityAction>();
	UtilityAction->ResultScore = -1.f;
	UtilityAction->SetCachedAIComponent(UtilityAIComponent);
	UMockTestUtilityAxis* TestAxis_A = NewObject<UMockTestUtilityAxis>();
	UMockTestUtilityAxis* TestAxis_B = NewObject<UMockTestUtilityAxis>();
	
	UMockTestUtilityArrayAxis* TestArrayAxis_A = NewObject<UMockTestUtilityArrayAxis>();
	UMockTestUtilityArrayAxis* TestArrayAxis_B = NewObject<UMockTestUtilityArrayAxis>();
	
	TestAxis_A->WantedValue = 0.9f;
	TestAxis_B->WantedValue = 0.8f;
	TArray<float> TestArrayAxisValue_A;
	TArray<float> TestArrayAxisValue_B;
	
	TestArrayAxisValue_A.Add(0.8f);
	TestArrayAxisValue_A.Add(0.7f);
	
	TestArrayAxisValue_B.Add(0.8f);
	TestArrayAxisValue_B.Add(0.9f);
	
	TestArrayAxis_A->WantedValue = TestArrayAxisValue_A;
	TestArrayAxis_B->WantedValue = TestArrayAxisValue_B;
	
	TArray<TObjectPtr<UBattleUtilityAxis>>& AxisArray = UtilityAction->GetAxisArray();
	TArray<TObjectPtr<UBattleUtilityArrayAxis>>& ArrayAxisArray = UtilityAction->GetArrayAxisArray();
	TArray<EAxisType>& AxisTypeArray = UtilityAction->GetAxisTypeArray();
	
	AxisArray.Add(TestAxis_A);
	AxisArray.Add(TestAxis_B);
	AxisTypeArray.Add(EAxisType::Target);
	ArrayAxisArray.Add(TestArrayAxis_A);
	ArrayAxisArray.Add(TestArrayAxis_B);
	
	
	ABattleCharacterBase* TestActor_A = GetWorld()->SpawnActor<ABattleCharacterBase>();
	ABattleCharacterBase* TestActor_B = GetWorld()->SpawnActor<ABattleCharacterBase>();
	UConsiderationFactors* ConsiderList = NewObject<UConsiderationFactors>(UtilityAIComponent);
	UtilityAIComponent->ConsiderList = ConsiderList;
	
	ConsiderList->TargetActors.Add(TestActor_A);
	ConsiderList->TargetActors.Add(TestActor_B);
	
	float Score = UtilityAction->EvaluateScore(ConsiderList);
	float WantedScore = 0.9*0.8*0.8*0.8; 
	
	
	TestEqual(TEXT("Axis는 곱해지고 ArrayAxis는 가장 값이 높게 나오는 Index의 것으로 선택 되어야 함"),Score, WantedScore);
	
	if (UtilityAction->GetBestTargets().IsEmpty())
	{
		return false;
	}
	
	TestEqual(TEXT("ArrayAxisType이 하나이기에 하나의 BestTarget만 선택되어야 함"), UtilityAction->GetBestTargets().Num(), 1);
	TestTrue(TEXT("선택된 BestTarget은 ArrayAxis의 곱 점수가 가장 높은 A가 선택되어야 함"), UtilityAction->GetBestTargets()[0] == TestActor_A);
	
	TestArrayAxisValue_A[0] = 0.5f;
	TestArrayAxisValue_A[1] = 0.6f;
	TestArrayAxisValue_B[0] = 0.7f;
	TestArrayAxisValue_B[1] = 0.6f;
	
	TestArrayAxis_A->WantedValue = TestArrayAxisValue_A;
	TestArrayAxis_B->WantedValue = TestArrayAxisValue_B;
	
	Score = UtilityAction->EvaluateScore(ConsiderList);
	WantedScore = 0.9*0.8*0.6*0.6;
	
	TestEqual(TEXT("일반 Axis는 곱해지고, ArrayAxis는 가장 값이 높게 나오는 Index의 것으로 선택되어야 함"), Score, WantedScore);
	
	if (UtilityAction->GetBestTargets().IsEmpty())
	{
		return false;
	}
	
	TestEqual(TEXT("ArrayAxisType이 하나이기에 하나의 BestTarget만 선택되어야 함"), UtilityAction->GetBestTargets().Num(), 1);
	TestTrue(TEXT("선택된 BestTarget은 ArrayAxis의 곱 점수가 가장 높은 B가 선택되어야 함"), UtilityAction->GetBestTargets()[0] == TestActor_B);
	
	
	
	
	
	return true;
}


bool FUtilityAIComponentTest::RunTest(const FString& Parameters)
{
	if (!SetupWorld())
	{
		return false;
	}
	
	ON_SCOPE_EXIT{
		ClearWorld();
	};
	
	
	ABattleEnemyCharacter* TestEnemy = GetWorld()->SpawnActor<ABattleEnemyCharacter>();
	UBattleUtilityAIComponent* UtilityAIComponent = NewObject<UBattleUtilityAIComponent>(TestEnemy);
	
	TestEnemy->AddInstanceComponent(UtilityAIComponent);
	UtilityAIComponent->RegisterComponent();
	
	
	ABattleCharacterBase* SelectedTarget = GetWorld()->SpawnActor<ABattleCharacterBase>();
	UConsiderationFactors* ConsiderList = NewObject<UConsiderationFactors>(UtilityAIComponent);
	UtilityAIComponent->ConsiderList = ConsiderList;
	ConsiderList->MyCharacter = TestEnemy;
	ConsiderList->SelectedTarget = SelectedTarget;
	ConsiderList->UtilityAIComponent = UtilityAIComponent;
	
	TArray<float>& LastActiveTime = UtilityAIComponent->GetLastActiveTime();
	LastActiveTime.Add(-1000.f);
	LastActiveTime.Add(-1000.f);
	
	TArray<TObjectPtr<UBattleUtilityAction>>& InstancedAction = UtilityAIComponent->GetInstancedActions();
	UMockTestUtilityAction* TestAction_A = NewObject<UMockTestUtilityAction>();
	UMockTestUtilityAction* TestAction_B = NewObject<UMockTestUtilityAction>();
	
	TestAction_A->SetAgeRate(0.5f);
	TestAction_A->SetAgeTime(10.f);
	TestAction_A->SetCoolTime(5.f);
	TestAction_A->SetMaxDistance(100.f);
	TestAction_A->SetMinDistance(0.0f);
	TestAction_A->SetDistanceSourceType(EDistanceSourceType::None);
	TestAction_A->ResultScore = 1.f;
	
	
	TestAction_B->SetAgeRate(0.5f);
	TestAction_B->SetAgeTime(10.f);
	TestAction_B->SetCoolTime(5.f);
	TestAction_B->SetMaxDistance(100.f);
	TestAction_B->SetMinDistance(10.f);
	TestAction_B->SetDistanceSourceType(EDistanceSourceType::None);
	TestAction_B->ResultScore = 0.5f;
	
	InstancedAction.Add(TestAction_A);
	InstancedAction.Add(TestAction_B);
	
	ConsiderList->CurTime = 0.0f;
	
	int SelectedActionIdx = UtilityAIComponent->CalcActionsScore();
	if (!InstancedAction.IsValidIndex(SelectedActionIdx))
	{
		return false;
	}
	TestTrue(TEXT("Age, CoolTime 등의 요소가 영향이 없으면 ResultScore가 높은 A가 선택되어야 함"),InstancedAction[SelectedActionIdx] == TestAction_A);
	
	LastActiveTime[0] = 0.0f;
	ConsiderList->CurTime = 3.f;
	SelectedActionIdx = UtilityAIComponent->CalcActionsScore();
	if (!InstancedAction.IsValidIndex(SelectedActionIdx))
	{
		return false;
	}
	TestTrue(TEXT("CoolTime이 안지났으면 아예 후보에도 안올라가기 때문에 B가 선택되어야 함"), InstancedAction[SelectedActionIdx] == TestAction_B);
	
	TestAction_A->ResultScore = 0.6f;
	// 이러면 흐른 시간은 6초, AgeTime은 10초이기 때문에 6/10 인 0.6이 AgeAlpha로 결정되고, AgeRate인 0.5와 1.0 간의 AlphaLerp값으로 최종적으로 0.8의 값이 CurScore에 곱해짐
	// 따라서 최종적으로 A의 스코어는 0.6 * 0.8로 0.48이기에 TestAction_B가 이겨야 함
	ConsiderList->CurTime = 6.f;
	SelectedActionIdx = UtilityAIComponent->CalcActionsScore();
	if (!InstancedAction.IsValidIndex(SelectedActionIdx))
	{
		return false;
	}
	TestTrue(TEXT("AgeTime이 안지나서 AgeRate가 적용되고, 이는 최종 결과가 B가 더 크기에 B가 선택되어야 함"), InstancedAction[SelectedActionIdx] == TestAction_B);
	
	TestAction_A->SetDistanceSourceType(EDistanceSourceType::SelectedTarget);
	TestAction_B->SetDistanceSourceType(EDistanceSourceType::SelectedTarget);
	SelectedActionIdx = UtilityAIComponent->CalcActionsScore();
	if (!InstancedAction.IsValidIndex(SelectedActionIdx))
	{
		return false;
	}
	TestTrue(TEXT("A와 B 모두 SelectedTarget 기준으로 Distance를 측정하지만 B의 경우에는 MinDistance가 10이기에 현재 둘의 거리가 0인 것은 허용되지 않아서 A가 선택되어야 함"), InstancedAction[SelectedActionIdx] == TestAction_A);
	
	
	
	
	
	
	return true;
}


bool FConsiderationFactorsTest::RunTest(const FString& Parameters)
{
	if (!SetupWorld())
	{
		return false;
	}
	
	ON_SCOPE_EXIT{
		ClearWorld();
	};
	
	
	UConsiderationFactors* ConsiderList = NewObject<UConsiderationFactors>();
	
	ConsiderList->bIsInCombat = true;
	
	ConsiderList->MyHp = 0.5f;
	ConsiderList->bBreakLeftLeg = false;
	ConsiderList->bBreakRightLeg = false;
	ConsiderList->BestCombatTime = 10.f;
	ConsiderList->CurTime = 10.f;
	ConsiderList->CombatStartTime =0.f; 
	ConsiderList->MyCharacterAreaFlag  = AREA_HighArea;
	ConsiderList->PoisonAmount = 1.f;
	ConsiderList->ElectricAmount = 1.f;
	
	/*
	 * CombatPotential - [Hp], Break Left And Right, CombatDuration, PositionDisadvantage, PoisonAmount, ElectricAmount
	 */

	const float OriginCombatPotential = ConsiderList->GetMyCombatPotential();
	ConsiderList->MyHp = 0.2f;
	TestTrue(TEXT("Hp가 낮아지면 CombatPotential의 값은 작아져야함"), OriginCombatPotential > ConsiderList->GetMyCombatPotential());
	ConsiderList->MyHp = 0.5f;
	
	ConsiderList->bBreakLeftLeg = true;
	TestTrue(TEXT("왼쪽 다리가 부숴지면, CombatPotential의 값은 작아져야함"), OriginCombatPotential > ConsiderList->GetMyCombatPotential());
	ConsiderList->bBreakLeftLeg = false;
	
	ConsiderList->bBreakRightLeg = true;
	TestTrue(TEXT("오른쪽 다리가 부숴지면, CombatPotential의 값은 작아져야함"), OriginCombatPotential > ConsiderList->GetMyCombatPotential());
	ConsiderList->bBreakRightLeg = false;
	
	ConsiderList->CurTime = 20.f;
	TestTrue(TEXT("너무 오래 싸우면 CombatPotential의 값은 작아져야함"), OriginCombatPotential > ConsiderList->GetMyCombatPotential());
	ConsiderList->CurTime = 10.f;
	
	ConsiderList->MyCharacterAreaFlag = AREA_LowArea;
	TestTrue(TEXT("안좋은 자리로 이동하면 CombatPotential의 값은 작아져야함"), OriginCombatPotential > ConsiderList->GetMyCombatPotential());
	ConsiderList->MyCharacterAreaFlag = AREA_HighArea;
	
	ConsiderList->PoisonAmount = 0.2;
	TestTrue(TEXT("PoisonAmount가 낮아지면 CombatPotential의 값은 작아져야함"), OriginCombatPotential > ConsiderList->GetMyCombatPotential());
	ConsiderList->PoisonAmount = 1.f;
	
	ConsiderList->ElectricAmount = 0.2;
	TestTrue(TEXT("ElectricAmount가 낮아지면 CombatPotential의 값은 작아져야함"), OriginCombatPotential > ConsiderList->GetMyCombatPotential());
	ConsiderList->ElectricAmount = 1.f;
	
	
	/*
	 * TreatScore - [NearByEnemyPressure], CombatDuration, PositionalDisadvantage, TargetSightClear, SurroundedRisk
	 */
	
	ABattleCharacterBase* Character = GetWorld()->SpawnActor<ABattleCharacterBase>();
	ConsiderList->MyCharacter = Character;
	ConsiderList->ThreatCharacterNum = 2;
	ConsiderList->TargetDistances.Empty();
	ConsiderList->TargetDistances.Add(0.2f);
	ConsiderList->TargetDistances.Add(1.f);
	ConsiderList->BestCombatTime = 10.f;
	ConsiderList->CurTime = 10.f;
	ConsiderList->CombatStartTime =0.f;
	ConsiderList->MyCharacterAreaFlag  = AREA_LowArea;
	ConsiderList->bIsSelectedTargetSightClear = false;
	
	AMockTestBattleCharacterBase* TestActor_A = GetWorld()->SpawnActor<AMockTestBattleCharacterBase>();
	AMockTestBattleCharacterBase* TestActor_B = GetWorld()->SpawnActor<AMockTestBattleCharacterBase>();
	
	TestActor_A->SetActorLocation(FVector(5.f, 5.f, 0.f));
	TestActor_B->SetActorLocation(FVector(-5.f, -5.f, 0.f));
	
	ConsiderList->TargetActors.Add(TestActor_A);
	ConsiderList->TargetActors.Add(TestActor_B);
	
	
	const float OriginThreatScore = ConsiderList->GetThreatScore();
	
	ConsiderList->TargetDistances.Empty();
	ConsiderList->TargetDistances.Add(1.f);
	ConsiderList->TargetDistances.Add(1.f);
	TestTrue(TEXT("적과의 거리가 멀면 위협 수치가 떨어짐"), OriginThreatScore > ConsiderList->GetThreatScore());
	ConsiderList->TargetDistances.Empty();
	ConsiderList->TargetDistances.Add(0.2f);
	ConsiderList->TargetDistances.Add(1.f);
	
	ConsiderList->CurTime = 20.f;
	TestTrue(TEXT("전투시간이 길어지면 위협 수치가 커짐"), OriginThreatScore < ConsiderList->GetThreatScore());
	ConsiderList->CurTime = 10.f;
	
	ConsiderList->MyCharacterAreaFlag = AREA_HighArea;
	TestTrue(TEXT("내 자리가 좋으면 위협 수치가 떨어짐"), OriginThreatScore > ConsiderList->GetThreatScore());
	ConsiderList->MyCharacterAreaFlag = AREA_LowArea;
	
	ConsiderList->bIsSelectedTargetSightClear = true;
	TestTrue(TEXT("선택된 적이 내 눈앞에 보이면 위협 수치가 떨어짐"), OriginThreatScore > ConsiderList->GetThreatScore());
	ConsiderList->bIsSelectedTargetSightClear = false;
	
	AMockTestBattleCharacterBase* TestActor_C = GetWorld()->SpawnActor<AMockTestBattleCharacterBase>();
	AMockTestBattleCharacterBase* TestActor_D = GetWorld()->SpawnActor<AMockTestBattleCharacterBase>();
	
	TestActor_C->SetActorLocation(FVector(5.f, -5.f, 0.f));
	TestActor_D->SetActorLocation(FVector(-5.f, 5.f, 0.f));
	ConsiderList->TargetActors.Add(TestActor_C);
	ConsiderList->TargetActors.Add(TestActor_D);
	TestTrue(TEXT("주변에 둘러싼 적이 많아지면 위협 수치가 커짐"), OriginThreatScore < ConsiderList->GetThreatScore());
	ConsiderList->TargetActors.Empty();
	
	/*
	 * NeedCombatReposition - PositionDisadvantage, SurroundedRisk, NearByEnemyPressure
	 */
	
	ConsiderList->MyCharacterAreaFlag = AREA_LowArea;
	ConsiderList->ThreatCharacterNum = 2;
	ConsiderList->TargetDistances.Empty();
	ConsiderList->TargetDistances.Add(1.f);
	ConsiderList->TargetActors.Empty();
	ConsiderList->TargetActors.Add(TestActor_A);
	ConsiderList->TargetActors.Add(TestActor_B);
	
	const float OriginNeedCombatReposition = ConsiderList->GetNeedCombatReposition();
	
	ConsiderList->MyCharacterAreaFlag = AREA_HighArea;
	TestEqual(TEXT("현 위치가 좋으면 이동할 필요가 없음"), ConsiderList->GetNeedCombatReposition(), 0.0f);
	ConsiderList->MyCharacterAreaFlag = AREA_LowArea;
	
	ConsiderList->TargetDistances.Add(0.2f);
	TestTrue(TEXT("적의 압박이 커지면, 이동할 필요성이 높아짐"), OriginNeedCombatReposition < ConsiderList->GetNeedCombatReposition());
	ConsiderList->TargetDistances.Empty();
	ConsiderList->TargetDistances.Add(1.f);
	
	ConsiderList->TargetActors.Add(TestActor_C);
	TestTrue(TEXT("둘러싼 적이 늘어날수록, 이동할 필요성이 높아짐"), OriginNeedCombatReposition < ConsiderList->GetNeedCombatReposition());
	ConsiderList->TargetActors.Empty();
	
	/*
	 * NeedCombatAreaChange - [CombatDuration], ThreatScore, CombatPotential, PoisonAmount, ElectricAmount
	 */
	
	ConsiderList->BestCombatTime = 10.f;
	ConsiderList->CurTime = 8.f;
	ConsiderList->CombatStartTime = 0.f;
	ConsiderList->MyHp = 0.5f;
	ConsiderList->ThreatCharacterNum = 2;
	ConsiderList->TargetDistances.Empty();
	ConsiderList->TargetDistances.Add(0.2f);
	ConsiderList->TargetDistances.Add(1.f);
	ConsiderList->PoisonAmount = 0.8f;
	ConsiderList->ElectricAmount = 0.8f;
	
	const float OriginNeedCombatAreaChange = ConsiderList->GetNeedCombatAreaChange();
	
	ConsiderList->CurTime = 15.f;
	TestTrue(TEXT("전투 시간이 길어지면 AreaChange의 필요성이 높아짐"), OriginNeedCombatAreaChange < ConsiderList->GetNeedCombatAreaChange());
	ConsiderList->CurTime = 8.f;
	
	ConsiderList->MyHp = 0.1f;
	TestTrue(TEXT("CombatPotential이 낮아지면 AreaChange의 필요성이 높아짐"), OriginNeedCombatAreaChange < ConsiderList->GetNeedCombatAreaChange());
	ConsiderList->MyHp = 0.5f;
	
	ConsiderList->TargetDistances.Add(0.2f);
	TestTrue(TEXT("위협 수치가 늘어나면 AreaChange의 필요성이 높아짐"),OriginNeedCombatAreaChange < ConsiderList->GetNeedCombatAreaChange());
	ConsiderList->TargetDistances.Empty();
	ConsiderList->TargetDistances.Add(0.2f);
	ConsiderList->TargetDistances.Add(1.f);
	
	
	ConsiderList->PoisonAmount = 0.1f;
	TestTrue(TEXT("PoisonAmount가 작아지면 AreaChange의 필요성이 높아짐"),OriginNeedCombatAreaChange < ConsiderList->GetNeedCombatAreaChange());
	ConsiderList->PoisonAmount = 0.8f;
	
	ConsiderList->ElectricAmount = 0.1f;
	TestTrue(TEXT("ElectircAmount가 작아지면 AreaChange의 필요성이 높아짐"),OriginNeedCombatAreaChange < ConsiderList->GetNeedCombatAreaChange());
	ConsiderList->ElectricAmount = 0.8f;
	
	/*
	 * TargetPriority - [-TargetDistances], DamageRate, TargetHps 
	 */
	
	
	ConsiderList->TargetActors.Empty();
	ConsiderList->TargetActors.Add(TestActor_A);
	ConsiderList->TargetActors.Add(TestActor_B);
	ConsiderList->TargetActors.Add(TestActor_C);
	ConsiderList->TargetActors.Add(TestActor_D);
	
	ConsiderList->TargetDistances.Empty();
	ConsiderList->TargetDistances.Add(0.5f);
	ConsiderList->TargetDistances.Add(0.5f);
	ConsiderList->TargetDistances.Add(0.5f);
	ConsiderList->TargetDistances.Add(0.5f);
	
	ConsiderList->TargetHps.Empty();
	ConsiderList->TargetHps.Add(0.8f);
	ConsiderList->TargetHps.Add(0.8f);
	ConsiderList->TargetHps.Add(0.8f);
	ConsiderList->TargetHps.Add(0.8f);
	
	ConsiderList->TotalDamage = 100.f;
	ConsiderList->DamageStack.Add({TestActor_A, 50.f});
	ConsiderList->DamageStack.Add({TestActor_B, 30.f});
	ConsiderList->DamageStack.Add({TestActor_C, 20.f});
	ConsiderList->DamageStack.Add({TestActor_D, 0.f});
	
	
	TArray<float> CurTargetPriority = ConsiderList->GetTargetPriority();
	
	TestTrue(TEXT("A가 B보다 딜을 더 많이 넣었기에 우선순위가 높음"), CurTargetPriority[0] > CurTargetPriority[1]);
	TestTrue(TEXT("B가 C보다 딜을 더 많이 넣었기에 우선순위가 높음"), CurTargetPriority[1] > CurTargetPriority[2]);
	TestTrue(TEXT("C가 D보다 딜을 더 많이 넣었기에 우선순위가 높음"), CurTargetPriority[2] > CurTargetPriority[3]);
	ConsiderList->DamageStack[TestActor_A] = 25.f;
	ConsiderList->DamageStack[TestActor_B] = 25.f;
	ConsiderList->DamageStack[TestActor_C] = 25.f;
	ConsiderList->DamageStack[TestActor_D] = 25.f;
	

	
	ConsiderList->TargetHps[0] = 0.2f;
	CurTargetPriority = ConsiderList->GetTargetPriority();
	TestTrue(TEXT("A의 Hp가 아주 낮기 때문에 우선순위가 더 높아짐"), CurTargetPriority[0] > CurTargetPriority[1]);
	ConsiderList->TargetHps[0] = 0.8f;
	
	
	ConsiderList->TargetDistances[0] = 0.2f;
	ConsiderList->TargetDistances[1] = 0.4f;
	ConsiderList->TargetDistances[2] = 0.6f;
	ConsiderList->TargetDistances[3] = 0.8f;
	CurTargetPriority = ConsiderList->GetTargetPriority();
	TestTrue(TEXT("A가 B보다 더 가깝기 때문에 우선순위가 높음"), CurTargetPriority[0] > CurTargetPriority[1]);
	TestTrue(TEXT("B가 C보다 더 가깝기 때문에 우선순위가 높음"), CurTargetPriority[1] > CurTargetPriority[2]);
	TestTrue(TEXT("C가 D보다 더 가깝기 때문에 우선순위가 높음"), CurTargetPriority[2] > CurTargetPriority[3]);
	
	
	/*
	 * TargetWeakness - [-TargetHps], PoisonState, TargetDistances
	 */
	
	ConsiderList->TargetActors.Empty();
	ConsiderList->TargetActors.Add(TestActor_A);
	ConsiderList->TargetActors.Add(TestActor_B);
	ConsiderList->TargetActors.Add(TestActor_C);
	
	ConsiderList->TargetHps.Empty();
	ConsiderList->TargetHps.Add(0.5f);
	ConsiderList->TargetHps.Add(0.5f);
	ConsiderList->TargetHps.Add(0.5f);
	
	ConsiderList->TargetDistances.Empty();
	ConsiderList->TargetDistances.Add(0.5f);
	ConsiderList->TargetDistances.Add(0.5f);
	ConsiderList->TargetDistances.Add(0.5f);
	
	UBattleAbilitySystemComponent* TestASC_A = NewObject<UBattleAbilitySystemComponent>(TestActor_A);
	TestActor_A->AddInstanceComponent(TestASC_A);
	TestASC_A->RegisterComponent();
	TestASC_A->InitAbilityActorInfo(TestActor_A, TestActor_A);
	TestActor_A->ASC = TestASC_A;

	TestASC_A->AddLooseGameplayTag(FBattleGameplayTags::Get().Status_Poisoned);
	TArray<float> CurTargetWeakness = ConsiderList->GetTargetWeakness();
	TestTrue(TEXT("독에 걸리면 더 약한 상태로 판단"), CurTargetWeakness[0] > CurTargetWeakness[1]);
	TestASC_A->RemoveLooseGameplayTag(FBattleGameplayTags::Get().Status_Poisoned);
	
	ConsiderList->TargetHps[0] = 0.2f;
	ConsiderList->TargetHps[1] = 0.5f;
	ConsiderList->TargetHps[2] = 0.8f;
	CurTargetWeakness = ConsiderList->GetTargetWeakness();
	TestTrue(TEXT("A는 B보다 Hp가 더 낮기 때문에 더 약한 상태로 판단"), CurTargetWeakness[0] > CurTargetWeakness[1]);
	TestTrue(TEXT("B는 C보다 Hp가 더 낮기 때문에 더 약한 상태로 판단"), CurTargetWeakness[1] > CurTargetWeakness[2]);
	ConsiderList->TargetHps[0] = 0.8f;
	ConsiderList->TargetHps[1] = 0.8f;
	ConsiderList->TargetHps[2] = 0.8f;
	
	
	ConsiderList->TargetDistances[0] = 0.5f;
	ConsiderList->TargetDistances[1] = 0.9f;
	CurTargetWeakness = ConsiderList->GetTargetWeakness();
	TestTrue(TEXT("B는 A보다 먼 거리에 있기에 A를 더 약한 상태로 판단"), CurTargetWeakness[0] > CurTargetWeakness[1]);
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	return true;
}




#endif
