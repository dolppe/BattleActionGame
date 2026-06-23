#pragma once

#include "BattleActionGame/Combat/BattleCombatManagerComponent.h"

#include "BattleCharacterTests.generated.h"

UCLASS()
class UMockTestBattleCombatManagerComponent : public UBattleCombatManagerComponent
{
	GENERATED_BODY()
public:
	virtual void OnHitEvent(const FBattleHitMessage& HitMessage) override;
	
	int HitEventCount;
	
};
