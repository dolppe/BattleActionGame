#pragma once

#include "BattleUtilityAxis.h"
#include "BattleUtilityAxis_EnvironmentConditions.generated.h"

UCLASS()
class UBattleUtilityAxis_HasTarget : public UBattleUtilityAxis
{
	GENERATED_BODY()
public:

	UBattleUtilityAxis_HasTarget();

	virtual float GetValue() override;

protected:

	
	
};

