#include "BattleItemExecution.h"

#include "BattleActionGame/BattleGameplayTags.h"
#include "BattleActionGame/AbilitySystem/Attributes/BattleCombatSet.h"
#include "BattleActionGame/AbilitySystem/Attributes/BattleHealthSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleItemExecution)


struct FItemStatics
{
	FGameplayEffectAttributeCaptureDefinition MaxHpDef;
	FGameplayEffectAttributeCaptureDefinition MaxStaminaDef;
	FGameplayEffectAttributeCaptureDefinition HealingDef;
	FGameplayEffectAttributeCaptureDefinition DamageDef;
	
	FGameplayEffectAttributeCaptureDefinition BaseHealDef;
	FGameplayEffectAttributeCaptureDefinition BaseDamageDef;
	FGameplayEffectAttributeCaptureDefinition AttackPowerDef;

	FItemStatics()
	{
		MaxHpDef = FGameplayEffectAttributeCaptureDefinition(UBattleHealthSet::GetMaxHealthAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
		MaxStaminaDef = FGameplayEffectAttributeCaptureDefinition(UBattleHealthSet::GetMaxStaminaAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
		HealingDef = FGameplayEffectAttributeCaptureDefinition(UBattleHealthSet::GetHealingAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
		DamageDef = FGameplayEffectAttributeCaptureDefinition(UBattleHealthSet::GetDamageAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
		
		BaseHealDef = FGameplayEffectAttributeCaptureDefinition(UBattleCombatSet::GetBaseHealAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
		BaseDamageDef = FGameplayEffectAttributeCaptureDefinition(UBattleCombatSet::GetBaseDamageAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
		AttackPowerDef = FGameplayEffectAttributeCaptureDefinition(UBattleCombatSet::GetAttackPowerAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
	}
};

static FItemStatics& ItemStatics()
{
	static FItemStatics Statics;
	return Statics;
}

UBattleItemExecution::UBattleItemExecution()
{
	RelevantAttributesToCapture.Add(ItemStatics().MaxHpDef);
	RelevantAttributesToCapture.Add(ItemStatics().MaxStaminaDef);
	RelevantAttributesToCapture.Add(ItemStatics().HealingDef);
	RelevantAttributesToCapture.Add(ItemStatics().DamageDef);

	RelevantAttributesToCapture.Add(ItemStatics().BaseHealDef);
	RelevantAttributesToCapture.Add(ItemStatics().BaseDamageDef);
	RelevantAttributesToCapture.Add(ItemStatics().AttackPowerDef);
}

void UBattleItemExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                                  FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
#if WITH_SERVER_CODE

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	float MaxHpMagnitude = Spec.GetSetByCallerMagnitude(FBattleGameplayTags::Get().GameplayEffect_Caller_MaxHp, false, -1);
	float MaxStaminaMagnitude = Spec.GetSetByCallerMagnitude(FBattleGameplayTags::Get().GameplayEffect_Caller_MaxStamina, false, -1);
	

	if (MaxHpMagnitude != -1)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UBattleHealthSet::GetHealthAttribute(), EGameplayModOp::Override,MaxHpMagnitude));
	}

	if (MaxStaminaMagnitude != -1)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UBattleHealthSet::GetStaminaAttribute(), EGameplayModOp::Override, MaxHpMagnitude));
	}

	
#endif
}
