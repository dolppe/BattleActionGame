#include "BattleUtilityAxis_EnvironmentConditions.h"

#include "BattleActionGame/Enemy/UtilityAI/BattleUtilityAIData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleUtilityAxis_EnvironmentConditions)

UBattleUtilityAxis_HasTarget::UBattleUtilityAxis_HasTarget()
{
	AxisType = EBattleConsiderType::Target;
}

float UBattleUtilityAxis_HasTarget::GetValue()
{

	return 0.0f;
}
