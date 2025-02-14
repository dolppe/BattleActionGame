#pragma once

#include "AttackCollisionMethod.h"
#include "AttackCollisionMethod_CircularAOE.generated.h"

UCLASS()
class UAttackCollisionMethod_CircularAOE : public UAttackCollisionMethod
{
	GENERATED_BODY()
public:
	
	virtual void StartCollisionCheck() override;
	virtual void EndCollisionCheck() override;
	virtual void SetCollisionData(UAttackCollisionData* InAttackCollisionData, UBattleGameplayAbility_Attack_Parent* InGameplayAbility) override;
	virtual void SendHitResults(const FHitResult& HitResult, const float HitTime) override;


protected:

	TArray<FVector> GetBestSpots();
	
};

