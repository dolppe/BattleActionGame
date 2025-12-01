#include "BattleDamageExecution.h"

#include "BattleActionGame/BattleGameplayTags.h"
#include "BattleActionGame/BattleLogChannels.h"
#include "BattleActionGame/AbilitySystem/Attributes/BattleCombatSet.h"
#include "BattleActionGame/AbilitySystem/Attributes/BattleEnemySet.h"
#include "BattleActionGame/AbilitySystem/Attributes/BattleHealthSet.h"
#include "BattleActionGame/Character/BattleCharacter.h"
#include "BattleActionGame/Enemy/BattleEnemyCharacter.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleDamageExecution)


struct FDamageStatics
{
	FGameplayEffectAttributeCaptureDefinition AttackPowerDef;
	FGameplayEffectAttributeCaptureDefinition ImpactResistanceDef;
	

	FDamageStatics()
	{
		AttackPowerDef = FGameplayEffectAttributeCaptureDefinition(UBattleCombatSet::GetAttackPowerAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
		ImpactResistanceDef = FGameplayEffectAttributeCaptureDefinition(UBattleCombatSet::GetImpactResistanceAttribute(), EGameplayEffectAttributeCaptureSource::Target, true);
	}
};

static FDamageStatics& DamageStatics()
{
	static FDamageStatics Statics;
	return Statics;
}

UBattleDamageExecution::UBattleDamageExecution()
{
	RelevantAttributesToCapture.Add(DamageStatics().AttackPowerDef);
}


void UBattleDamageExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	
#if WITH_SERVER_CODE

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;
	
	float AttackPower = 0.0f;
	float ImpactResistance = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().AttackPowerDef, EvaluateParameters, AttackPower);
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ImpactResistanceDef, EvaluateParameters, ImpactResistance);
	
	float BaseDamage = Spec.GetSetByCallerMagnitude(FBattleGameplayTags::Get().GameplayEffect_Data_BaseDamage, true, 1.0f);
	float AttackRate = Spec.GetSetByCallerMagnitude(FBattleGameplayTags::Get().GameplayEffect_Data_AttackRate, true, 1.0f);

	float TotalDamage = 0.0f;

	TotalDamage = BaseDamage + AttackPower*AttackRate;

	if (TotalDamage > 0.0f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UBattleHealthSet::GetDamageAttribute(), EGameplayModOp::Additive, TotalDamage));
	}

	if (UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent())
	{
		float ImpactPower = Spec.GetSetByCallerMagnitude(FBattleGameplayTags::Get().GameplayEffect_Data_ImpactPower, true, 1.0f);
		float TotalImpactDamage = ImpactPower - ImpactResistance;

		UE_LOG(LogBattle, Log, TEXT("TotalImpactDamage %f, %s => %s"),TotalImpactDamage, *ExecutionParams.GetSourceAbilitySystemComponent()->GetName(),*ExecutionParams.GetTargetAbilitySystemComponent()->GetName());

		if (TotalImpactDamage > 0.0f)
		{
			if (ABattleEnemyCharacter* TargetEnemy = Cast<ABattleEnemyCharacter>(TargetASC->GetAvatarActor()))
			{
				OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UBattleEnemySet::GetGroggyValueAttribute(), EGameplayModOp::Additive, TotalImpactDamage));
				OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UBattleEnemySet::GetPoiseValueAttribute(), EGameplayModOp::Additive, -TotalImpactDamage));
			}

			OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UBattleCombatSet::GetImpactDamageAttribute(), EGameplayModOp::Additive, TotalImpactDamage));
		}

	}

	
#endif
}
