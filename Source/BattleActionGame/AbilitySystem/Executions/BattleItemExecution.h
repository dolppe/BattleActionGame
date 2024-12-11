#pragma once

#include "GameplayEffectExecutionCalculation.h"
#include "BattleItemExecution.generated.h"

UCLASS()
class UBattleItemExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
public:

	UBattleItemExecution();

protected:

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
	
};

