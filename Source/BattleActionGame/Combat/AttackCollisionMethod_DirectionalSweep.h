#pragma once

#include "AttackCollisionMethod.h"
#include "AttackCollisionMethod_DirectionalSweep.generated.h"

UCLASS()
class UAttackCollisionMethod_DirectionalSweep : public UAttackCollisionMethod
{
	GENERATED_BODY()
public:

	virtual void StartCollisionCheck(TArray<FHitResult>& OutHitResult, UAttackCollisionData* AttackCollisionData) override;
	virtual void EndCollisionCheck() override;

	static void DrawDebugWithStart(USkeletalMeshComponent* MeshComp, UAttackCollisionData* AttackCollisionData);
	
};

