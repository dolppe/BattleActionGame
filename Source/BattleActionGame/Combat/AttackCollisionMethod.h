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

struct FAttackCollisionDebugSpec
{
	TArray<FVector> StartArr;
	TArray<FVector> EndArr;
	
	bool IsValid() const
	{
		if (StartArr.IsEmpty())
		{
			return false;
		}
		if (EndArr.IsEmpty())
		{
			return false;
		}
		return true;
	}
	
};


UCLASS(Abstract)
class UAttackCollisionMethod : public UObject
{
	GENERATED_BODY()
public:
	
	virtual void StartCollisionCheck(TArray<FHitResult>& OutHitResult, UAttackCollisionData* AttackCollisionData, FAttackCollisionDebugSpec& OutCollisionDebugSpec);
	virtual void EndCollisionCheck();
	virtual bool IsNeedTick();
	virtual void TickCollisionCheck(TArray<FHitResult>& OutHitResult, UAttackCollisionData* AttackCollisionData, float FrameDeltaTime, FAttackCollisionDebugSpec& OutCollisionDebugSpec);
	
	void SetCharacter(ACharacter* InCharacter)
	{
		Character = InCharacter;
	}

protected:

	ACharacter* Character = nullptr;
	
};
