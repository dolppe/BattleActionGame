#pragma once

#include "AttackCollisionMethod.h"
#include "AttackCollisionMethod_DirectionalSweep.generated.h"

UCLASS()
class UAttackCollisionMethod_DirectionalSweep : public UAttackCollisionMethod
{
	GENERATED_BODY()
public:

	virtual void StartCollisionCheck() override;
	virtual void EndCollisionCheck() override;
	virtual void SetCollisionData(UAttackCollisionData* InAttackCollisionData, UBattleGameplayAbility_Attack_Parent* InGameplayAbility) override;
	virtual void SendHitResults(const FHitResult& HitResult, const float HitTime) override;
	
};

