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
	
	virtual void StartCollisionCheck(TArray<FHitResult>& OutHitResult, UAttackCollisionData* AttackCollisionData);
	virtual void EndCollisionCheck();
	virtual bool IsNeedTick();
	virtual void TickCollisionCheck(TArray<FHitResult>& OutHitResult, UAttackCollisionData* AttackCollisionData, float FrameDeltaTime);

	void SetCharacter(ACharacter* InCharacter)
	{
		Character = InCharacter;
	}

protected:

	ACharacter* Character = nullptr;
	
};
