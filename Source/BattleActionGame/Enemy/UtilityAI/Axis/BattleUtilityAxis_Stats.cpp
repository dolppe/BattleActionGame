#include "BattleUtilityAxis_Stats.h"

#include "BattleActionGame/Enemy/UtilityAI/BattleUtilityAIData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleUtilityAxis_Stats)

UBattleUtilityAxis_TargetHp::UBattleUtilityAxis_TargetHp()
{
	AxisType = EBattleConsiderType::TargetHp;
}

float UBattleUtilityAxis_TargetHp::GetValue()
{
	return 0.0f;
}
