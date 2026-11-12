
#if WITH_DEV_AUTOMATION_TESTS

#include "UtilityAISystemTests.h"

#include "BattleActorTestFixture.h"
#include "BattleCharacterTests.h"
#include "NavigationSystem.h"
#include "BattleActionGame/BattleGameplayTags.h"
#include "BattleActionGame/Character/BattleCharacterBase.h"
#include "BattleActionGame/Enemy/BattleEnemyCharacter.h"
#include "BattleActionGame/Enemy/UtilityAI/BattleConsiderationFactors.h"
#include "BattleActionGame/Enemy/UtilityAI/BattleUtilityAIComponent.h"
#include "BattleActionGame/Enemy/UtilityAI/BattleUtilityAIData.h"
#include "BattleActionGame/Enemy/UtilityAI/Actions/BattleUtilityAction_Search.h"
#include "BattleActionGame/Math/CalcFunctions.h"
#include "BattleActionGame/Navigation/NavArea/BattleNavArea.h"

IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(FUtilityActionSubClassTest, FBattleActorTestFixture,"BattleAction.UtilityAI.UtilityActionSubClass", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter);
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCalcFunctionTest, "BattleAction.Utility.CalcFunction", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter);
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FUtilityAIDataTest, "BattleAction.UtilityAI.UtilityAIData", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter);

bool FUtilityAIDataTest::RunTest(const FString& Parameters)
{
	FAxisConfig TestAxisConfig;
	
	TestAxisConfig.FunctionPreset = EAxisFunctionPreset::EaseOutHigh;
	UBattleUtilityAIData::ApplyAxisFunctionPreset(TestAxisConfig);
	
	TestEqual(TEXT("설정된 FunctionType으로 변경되어야 함"), TestAxisConfig.FunctionType, EAxisFunctionType::Quadratic);
	TestEqual(TEXT("설정된 Slope값으로 변경되어야 함"), TestAxisConfig.Slope, -1.0f);
	TestEqual(TEXT("설정된 Exponent값으로 변경되어야 함"), TestAxisConfig.Exponent, 2.0f);
	TestEqual(TEXT("설정된 VerticalShift값으로 변경되어야 함"), TestAxisConfig.VerticalShift, 1.0f);
	TestEqual(TEXT("설정된 HorizontalShift값으로 변경되어야 함"), TestAxisConfig.HorizontalShift, 1.0f);
	
	TestAxisConfig.FunctionPreset = EAxisFunctionPreset::Custom;
	TestAxisConfig.FunctionType = EAxisFunctionType::Logistic;
	TestAxisConfig.Slope = -100.f;
	TestAxisConfig.Exponent = 200.f;
	TestAxisConfig.VerticalShift = -200.f;
	TestAxisConfig.HorizontalShift = 100.f;
	
	UBattleUtilityAIData::ApplyAxisFunctionPreset(TestAxisConfig);
	
	TestEqual(TEXT("Custom은 아무것도 변경하면 안됨"), TestAxisConfig.FunctionType, EAxisFunctionType::Logistic);
	TestEqual(TEXT("Custom은 아무것도 변경하면 안됨"), TestAxisConfig.Slope, -100.0f);
	TestEqual(TEXT("Custom은 아무것도 변경하면 안됨"), TestAxisConfig.Exponent, 200.0f);
	TestEqual(TEXT("Custom은 아무것도 변경하면 안됨"), TestAxisConfig.VerticalShift, -200.0f);
	TestEqual(TEXT("Custom은 아무것도 변경하면 안됨"), TestAxisConfig.HorizontalShift, 100.0f);
	
	TestAxisConfig.FunctionPreset = EAxisFunctionPreset::LogitEndLowStrong;
	UBattleUtilityAIData::ApplyAxisFunctionPreset(TestAxisConfig);
	
	TestEqual(TEXT("설정된 FunctionType으로 변경되어야 함"), TestAxisConfig.FunctionType, EAxisFunctionType::Logit);
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
	

	MinValue = 0.0f;
	MaxValue = 1.0f;
	
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
	
	TArray<FVector> TestSpotList, ResultArr;
	float TestAreaRadius = 300.f;
	int TestAreaNum = 3;
	
	ResultArr = BattleUtilityActionHelper::FindBestSpotsWithGreedy(TestSpotList, TestAreaRadius, TestAreaNum);
	
	TestEqual(TEXT("전달된 Spot이 하나도 없으면 결과도 비어 있어야 함"), ResultArr.Num(), 0);
	
	FVector Spot_A = FVector(10.f,10.f,0.0f);
	
	TestSpotList.Add(Spot_A);
	
	ResultArr = BattleUtilityActionHelper::FindBestSpotsWithGreedy(TestSpotList, TestAreaRadius, TestAreaNum);
	
	TestEqual(TEXT("하나의 Spot만 전달되어도 해당 Spot을 3번 선택해야함"), ResultArr.Num(), 3);
	if (ResultArr.Num() != 3)
	{
		return false;
	}
	TestTrue(TEXT("하나의 Spot만 전달되면 해당 Spot만 선택 됨-첫번째 확인"), ResultArr[0].Equals(Spot_A));
	TestTrue(TEXT("하나의 Spot만 전달되면 해당 Spot만 선택 됨-두번째 확인"), ResultArr[1].Equals(Spot_A));
	TestTrue(TEXT("하나의 Spot만 전달되면 해당 Spot만 선택 됨-세번째 확인"), ResultArr[2].Equals(Spot_A));
	
	FVector Spot_B = FVector(100.f, 100.f, 0.0f);
	TestSpotList.Add(Spot_B);
	TestAreaRadius = 10.f;
	
	ResultArr = BattleUtilityActionHelper::FindBestSpotsWithGreedy(TestSpotList, TestAreaRadius, TestAreaNum);
	
	TestEqual(TEXT("두개의 Spot이 전달되고, 두 Spot 사이의 거리가 AreaRadius보다 멀어도 3개의 스팟이 선택되어야 함"), ResultArr.Num(), 3);
	if (ResultArr.Num() != 3)
	{
		return false;
	}
	TestTrue(TEXT("두개의 Spot이 전달되고, 두 Spot 사이의 거리가 AreaRadius보다 멀면 그냥 두 스팟만 선택됨-첫번째"), ResultArr[0].Equals(Spot_A));
	TestTrue(TEXT("두개의 Spot이 전달되고, 두 Spot 사이의 거리가 AreaRadius보다 멀면 그냥 두 스팟만 선택됨-두번째"), ResultArr[1].Equals(Spot_B));
	TestTrue(TEXT("두개의 Spot이 전달되고, 두 Spot 사이의 거리가 AreaRadius보다 멀면 그냥 두 스팟만 선택됨-세번째"), ResultArr[0].Equals(Spot_A));
	
	TestAreaNum = 5;
	FVector Spot_C = FVector(12.f, 12.f,0.0f);
	TestSpotList.Add(Spot_C);
	
	ResultArr = BattleUtilityActionHelper::FindBestSpotsWithGreedy(TestSpotList, TestAreaRadius, TestAreaNum);
	
	TArray<FVector> NewSpots;
	
	int Count = 0;
	
	for (FVector& ResultSpot : ResultArr)
	{
		if (ResultSpot.Equals(Spot_A) || ResultSpot.Equals(Spot_B) || ResultSpot.Equals(Spot_C))
		{
			Count++;
			continue;
		}
		
	}
	TestEqual(TEXT("신규 Spot의 개수보다 AreaNum이 더 많이 남으면 그건 입력으로 받은 Spot으로 채움"), Count, 5);
	
	
	return true;
}





#endif
void UMockConsiderationFactors::ChangeFactorValue(EBattleConsiderType Consider, float InFactorValue)
{
	Factors.FindOrAdd(Consider) = InFactorValue;
}

void UMockConsiderationFactors::ChangeMultiFactorValue(EBattleConsiderType Consider, TArray<float> InMultiFactorValues)
{
	MultiFactors.FindOrAdd(Consider) = InMultiFactorValues;
}
