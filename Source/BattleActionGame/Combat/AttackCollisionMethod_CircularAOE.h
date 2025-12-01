#pragma once

#include "AttackCollisionMethod.h"
#include "BattleGameplayAbility_TargetedAttack.h"
#include "AttackCollisionMethod_CircularAOE.generated.h"

UCLASS()
class UAttackCollisionMethod_CircularAOE : public UAttackCollisionMethod
{
	GENERATED_BODY()
public:
	
	virtual void StartCollisionCheck(TArray<FHitResult>& OutHitResult, UAttackCollisionData* AttackCollisionData) override;
	virtual void EndCollisionCheck() override;

protected:
	
};

