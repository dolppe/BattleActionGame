#include "AttackCollisionMethod.h"
#include "BattleGameplayAbility_Attack_Parent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AttackCollisionMethod)

void UAttackCollisionMethod::StartCollisionCheck(TArray<FHitResult>& OutHitResult,
	UAttackCollisionData* AttackCollisionData)
{
}

void UAttackCollisionMethod::EndCollisionCheck()
{
}

bool UAttackCollisionMethod::IsNeedTick()
{
	return false;
}

void UAttackCollisionMethod::TickCollisionCheck(TArray<FHitResult>& OutHitResult,
	UAttackCollisionData* AttackCollisionData, float FrameDeltaTime)
{
}

