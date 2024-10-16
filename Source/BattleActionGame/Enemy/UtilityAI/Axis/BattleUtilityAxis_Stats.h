#pragma once


#include "BattleUtilityAxis.h"
#include "BattleUtilityAxis_Stats.generated.h"

UCLASS()
class UBattleUtilityAxis_TargetHp : public UBattleUtilityAxis
{
	GENERATED_BODY()
public:

	UBattleUtilityAxis_TargetHp();

	virtual float GetValue() override;

protected:

	
	
};

