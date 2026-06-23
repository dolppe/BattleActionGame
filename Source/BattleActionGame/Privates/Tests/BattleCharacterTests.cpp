
#if WITH_DEV_AUTOMATION_TESTS

#include "BattleCharacterTests.h"




#endif

void UMockTestBattleCombatManagerComponent::OnHitEvent(const FBattleHitMessage& HitMessage)
{
	HitEventCount++;
	Super::OnHitEvent(HitMessage);
}
