
#if WITH_DEV_AUTOMATION_TESTS

#include "BattleActorTestFixture.h"
#include "BattleActionGame/Enemy/BattleEnemyCharacter.h"

IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(FEnemyCharacterTest, FBattleActorTestFixture, "BattleAction.Enemy.EnemyCharacter", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter);


bool FEnemyCharacterTest::RunTest(const FString& Parameters)
{
	if (!SetupWorld())
	{
		return false;
	}
	
	ON_SCOPE_EXIT{
		ClearWorld();
	};
	
	ABattleEnemyCharacter* EnemyCharacter = GetWorld()->SpawnActor<ABattleEnemyCharacter>();
	
	TestNotNull(TEXT("EnemyCharacter Must be Spawned"), EnemyCharacter);
	
	if (!EnemyCharacter)
	{
		return false;
	}
	
	TestNotNull(TEXT("EnemyCharacter Has AbilitySystemComponent"), EnemyCharacter->GetAbilitySystemComponent());
	
	// 추후에 그로기 PoiseBreak가 잘 진행되는지, 태그랑 움직일 수 있는지 테스트할 것.
	
	return true;
}




#endif
