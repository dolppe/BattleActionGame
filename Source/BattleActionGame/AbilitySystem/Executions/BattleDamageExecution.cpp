#include "BattleDamageExecution.h"

#include "BattleActionGame/BattleGameplayTags.h"
#include "BattleActionGame/AbilitySystem/Attributes/BattleCombatSet.h"
#include "BattleActionGame/AbilitySystem/Attributes/BattleEnemySet.h"
#include "BattleActionGame/AbilitySystem/Attributes/BattleHealthSet.h"
#include "BattleActionGame/Enemy/BattleEnemyCharacter.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleDamageExecution)


struct FDamageStatics
{
	FGameplayEffectAttributeCaptureDefinition BaseDamageDef;
	FGameplayEffectAttributeCaptureDefinition AttackPowerDef;

	FDamageStatics()
	{
		AttackPowerDef = FGameplayEffectAttributeCaptureDefinition(UBattleCombatSet::GetAttackPowerAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
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
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().AttackPowerDef, EvaluateParameters, AttackPower);
	
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
		if (ABattleEnemyCharacter* TargetEnemy= Cast<ABattleEnemyCharacter>(TargetASC->GetAvatarActor()))
		{
			float GroggyValue = Spec.GetSetByCallerMagnitude(FBattleGameplayTags::Get().GameplayEffect_Data_GroggyValue, true, 1.0f);
			OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UBattleEnemySet::GetGroggyValueAttribute(), EGameplayModOp::Additive, GroggyValue));
		}
	}

	
#endif
}
