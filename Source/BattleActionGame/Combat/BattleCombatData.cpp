#include "BattleCombatData.h"

#include "AttackCollisionMethod.h"
#include "BattleCombatManagerComponent.h"
#include "Misc/DataValidation.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleCombatData)

UAttackCollisionData_SocketBasedLineTrace::UAttackCollisionData_SocketBasedLineTrace()
{
	CollisionMethodType = ECollisionMethodType::SocketBasedLineTrace;
}

UAttackCollisionData_DirectionalSweep::UAttackCollisionData_DirectionalSweep()
{
	CollisionMethodType = ECollisionMethodType::DirectionalSweep;
}

UAttackCollisionData_CircularAOE::UAttackCollisionData_CircularAOE()
{
	CollisionMethodType = ECollisionMethodType::CircularAOE;
}


UBattleCombatData::UBattleCombatData()
{
}
#if WITH_EDITOR
EDataValidationResult UBattleCombatData::IsDataValid(class FDataValidationContext& Context)
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (BasicAttacks.IsEmpty() && ComboAttacks.IsEmpty() && ComboStrongAttacks.IsEmpty() && TargetedAttacks.IsEmpty() && SpawnDatas.IsEmpty())
	{
		Context.AddError(FText::FromString(TEXT("UBattleCombatData must have at least one AttackData")));
		Result = EDataValidationResult::Invalid;
	}
	else
	{
		for (FAttackData& BasicAttack : BasicAttacks)
		{
			if (BasicAttack.Montage == nullptr)
			{
				Context.AddError(FText::FromString(TEXT("BasicAttack-Montage must have not be null")));
				Result = EDataValidationResult::Invalid;
			}
		}
		for (FComboAttack& ComboAttack : ComboAttacks)
		{
			if (ComboAttack.Montage == nullptr)
			{
				Context.AddError(FText::FromString(TEXT("ComboAttack-Montage must have not be null")));
				Result = EDataValidationResult::Invalid;
			}
		}
		for (FAttackData& ComboStrongAttack : ComboStrongAttacks)
		{
			if (ComboStrongAttack.Montage == nullptr)
			{
				Context.AddError(FText::FromString(TEXT("ComboStrongAttack-Montage must have not be null")));
				Result = EDataValidationResult::Invalid;
			}
		}
		for (FAttackData& TargetedAttack : TargetedAttacks)
		{
			if (TargetedAttack.Montage == nullptr)
			{
				Context.AddError(FText::FromString(TEXT("TargetedAttack-Montage must have not be null")));
				Result = EDataValidationResult::Invalid;
			}
		}
		for (FSpecialSpawnData& SpawnData : SpawnDatas)
		{
			if (SpawnData.Montage == nullptr)
			{
				Context.AddError(FText::FromString(TEXT("SpawnData-Montage must have not be null")));
				Result = EDataValidationResult::Invalid;
			}
		}

	}

	return Result;
}
#endif
