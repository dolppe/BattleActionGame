#include "AttackCollisionMethod.h"
#include "BattleGameplayAbility_Attack_Parent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AttackCollisionMethod)

void UAttackCollisionMethod::StartCollisionCheck()
{
}

void UAttackCollisionMethod::EndCollisionCheck()
{
}

void UAttackCollisionMethod::SendHitResults(const TArray<FHitResult>& HitResults, const float HitTime)
{
	if (HitResults.Num() >0)
	{
		TArray<FHitResult> HitResultsForSend;
		for (const FHitResult& HitResult : HitResults)
		{
			auto Pred = [&HitResult](const FHitResult& OtherHitResult)
			{
				return OtherHitResult.HitObjectHandle == HitResult.HitObjectHandle;
			};

			if (!HitResultsForSend.ContainsByPredicate(Pred))
			{
				HitResultsForSend.Add(HitResult);
			}
		}
		OriginGameplayAbility->SelectHitCheck(HitResultsForSend, HitTime);
	}
}

void UAttackCollisionMethod::SetCollisionData(UAttackCollisionData* InAttackCollisionData,
                                              UBattleGameplayAbility_Attack_Parent* InGameplayAbility)
{
	AttackCollisionData = InAttackCollisionData;
	OriginGameplayAbility = InGameplayAbility;
}

