
#if WITH_DEV_AUTOMATION_TESTS

#include "BattleActionGame/Enemy/UtilityAI/BattleUtilityAIComponent.h"


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FConsiderationFactorsTest, "BattleAction.UtilityAI.UtilityAIComponent", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter);

bool FConsiderationFactorsTest::RunTest(const FString& Parameters)
{
	UConsiderationFactors* ConsiderList = NewObject<UConsiderationFactors>();
	
	ConsiderList->SelectedTarget == nullptr;
	TestEqual(TEXT("Target이 없으면 GetTraget은 0"), ConsiderList->GetTarget(), 0.0f);
	
	ConsiderList->MyHp = 1.0f;
	const float OriginCombatPotential = ConsiderList->GetMyCombatPotential();
	ConsiderList->MyHp = 0.2f;
	const float LowerHpCombatPotential = ConsiderList->GetMyCombatPotential();
	
	TestTrue(TEXT("Hp가 낮아지면 CombatPotential의 값은 작아져야 한다"), OriginCombatPotential > LowerHpCombatPotential);
	
	return true;
}




#endif
