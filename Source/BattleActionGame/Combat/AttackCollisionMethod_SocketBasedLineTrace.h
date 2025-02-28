#pragma once

#include "AttackCollisionMethod.h"
#include "AttackCollisionMethod_SocketBasedLineTrace.generated.h"

class UBattleAbilityTask_HitCheck;

UCLASS()
class UAttackCollisionMethod_SocketBasedLineTrace : public UAttackCollisionMethod
{
	GENERATED_BODY()
public:

	virtual void StartCollisionCheck() override;
	virtual void EndCollisionCheck() override;
	virtual void SetCollisionData(UAttackCollisionData* InAttackCollisionData, UBattleGameplayAbility_Attack_Parent* InGameplayAbility) override;

	UFUNCTION()
	virtual void SendHitResults(const FHitResult& HitResult, const float HitTime) override;

protected:
	
	UBattleAbilityTask_HitCheck* HitCheckTask;
	
};

