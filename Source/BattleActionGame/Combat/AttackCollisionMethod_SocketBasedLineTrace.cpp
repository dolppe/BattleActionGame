#include "AttackCollisionMethod_SocketBasedLineTrace.h"

#include "BattleAbilityTask_HitCheck.h"
#include "BattleCombatData.h"
#include "BattleGameplayAbility_Attack_Parent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AttackCollisionMethod_SocketBasedLineTrace)

void UAttackCollisionMethod_SocketBasedLineTrace::StartCollisionCheck()
{
	if (UAttackCollisionData_SocketBasedLineTrace* CollisionData = Cast<UAttackCollisionData_SocketBasedLineTrace>(AttackCollisionData))
	{
		HitCheckTask = UBattleAbilityTask_HitCheck::CreateTask(OriginGameplayAbility);
		HitCheckTask->SetHitCheckData(CollisionData->StartSocketName, CollisionData->EndSocketName, CollisionData->AttackRadius, CollisionData->CollisionChannel);
		HitCheckTask->OnHitChecked.AddDynamic(this, &ThisClass::SendHitResults);
		HitCheckTask->ReadyForActivation();
	}
}

void UAttackCollisionMethod_SocketBasedLineTrace::EndCollisionCheck()
{
	if (HitCheckTask)
	{
		HitCheckTask->EndTask();
	}
}

void UAttackCollisionMethod_SocketBasedLineTrace::SetCollisionData(UAttackCollisionData* InAttackCollisionData,
                                                                   UBattleGameplayAbility_Attack_Parent* InGameplayAbility)
{
	Super::SetCollisionData(InAttackCollisionData, InGameplayAbility);
}

void UAttackCollisionMethod_SocketBasedLineTrace::SendHitResults(const FHitResult& HitResult, const float HitTime)
{
	Super::SendHitResults(HitResult, HitTime);
}
