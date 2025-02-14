#include "AttackCollisionMethod.h"
#include "BattleGameplayAbility_Attack_Parent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AttackCollisionMethod)

void UAttackCollisionMethod::StartCollisionCheck()
{
}

void UAttackCollisionMethod::EndCollisionCheck()
{
}

void UAttackCollisionMethod::SendHitResults(const FHitResult& HitResult, const float HitTime)
{
	OriginGameplayAbility->SelectHitCheck(HitResult, HitTime);
}

void UAttackCollisionMethod::SetCollisionData(UAttackCollisionData* InAttackCollisionData,
                                              UBattleGameplayAbility_Attack_Parent* InGameplayAbility)
{
	AttackCollisionData = InAttackCollisionData;
	OriginGameplayAbility = InGameplayAbility;
}

