#pragma once

#include "AttackCollisionMethod.generated.h"

class UBattleGameplayAbility_Attack_Parent;
class UAttackCollisionData;

UENUM()
enum class ECollisionMethodType : uint8
{
	SocketBasedLineTrace,
	DirectionalSweep,
	CircularAOE,
	
};

UCLASS(Abstract)
class UAttackCollisionMethod : public UObject
{
	GENERATED_BODY()
public:
	
	virtual void StartCollisionCheck();
	virtual void EndCollisionCheck();
	virtual void SetCollisionData(UAttackCollisionData* InAttackCollisionData, UBattleGameplayAbility_Attack_Parent* InGameplayAbility);

protected:

	virtual void SendHitResults(const TArray<FHitResult>& HitResults, const float HitTime);

	UAttackCollisionData* AttackCollisionData;
	UBattleGameplayAbility_Attack_Parent* OriginGameplayAbility;
	
};
