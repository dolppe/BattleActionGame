#include "BattleUtilityAIData.h"

#include "BattleConsiderationFactors.h"
#include "Actions/BattleUtilityAction.h"
#include "BattleActionGame/Math/CalcFunctions.h"
#include "Misc/DataValidation.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleUtilityAIData)


FAxisConfig::FAxisConfig()
	: ConsiderType(EBattleConsiderType::MyHp),
	FunctionType(EAxisFunctionType::Quadratic)
{
}

FActionConfig::FActionConfig()
	: DistanceSourceType(EDistanceSourceType::None)
{
}

#if WITH_EDITOR

void UBattleUtilityAIData::PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);
	
	const FName ChangedName = PropertyChangedEvent.Property	? PropertyChangedEvent.Property->GetFName()	: NAME_None;

	// AxisConfig의 FunctionPreset이 변경되면
	if (ChangedName == GET_MEMBER_NAME_CHECKED(FAxisConfig, FunctionPreset))
	{
		const int32 ActionIdx = PropertyChangedEvent.GetArrayIndex("ActionConfigs");
		const int32 AxisIdx = PropertyChangedEvent.GetArrayIndex("AxisConfigs");
		
		if (!ActionConfigs.IsValidIndex(ActionIdx))
		{
			return;
		}
		
		FActionConfig& ActionConfig = ActionConfigs[ActionIdx];
		
		if (!ActionConfig.AxisConfigs.IsValidIndex(AxisIdx))
		{
			return;
		}
		
		FAxisConfig& AxisConfig = ActionConfig.AxisConfigs[AxisIdx];
		
		if (AxisConfig.FunctionPreset == EAxisFunctionPreset::Custom)
		{
			return;
		}
		
		// 설정된 Preset 값으로 세팅함
		ApplyAxisFunctionPreset(AxisConfig);
		
	}
	
}

EDataValidationResult UBattleUtilityAIData::IsDataValid(class FDataValidationContext& Context)
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (Consider.IsEmpty())
	{
		Context.AddError(FText::FromString(TEXT("UBattleUtilityAIData must have at least One Consider")));
		Result = EDataValidationResult::Invalid;
	}
	if (ActionConfigs.IsEmpty())
	{
		Context.AddError(FText::FromString(TEXT("UBattleUtilityAIData must have at least One ActionConfigs")));
		Result = EDataValidationResult::Invalid;
	}

	for (const FActionConfig& ActionConfig : ActionConfigs)
	{
		if (ActionConfig.ActionClass == nullptr)
		{
			Context.AddError(FText::FromString(TEXT("ActionConfig must have ActionClass")));
			Result = EDataValidationResult::Invalid;
		}
		if (ActionConfig.AxisConfigs.IsEmpty())
		{
			Context.AddError(FText::FromString(TEXT("ActionConfig must have at least one AxisConfig")));
			Result = EDataValidationResult::Invalid;
		}
		if (ActionConfig.DistanceSourceType != EDistanceSourceType::None && ActionConfig.MinDistance > ActionConfig.MaxDistance)
		{
			Context.AddError(FText::FromString(TEXT("MaxDistance must be greater than or equal to MinDistance")));
			Result = EDataValidationResult::Invalid;
		}
		
		for (const FAxisConfig& AxisConfig : ActionConfig.AxisConfigs)
		{
			if (!Consider.Contains(AxisConfig.ConsiderType))
			{
				Context.AddError(FText::FromString(TEXT("Consider must include Axis ConsiderType")));
				Result = EDataValidationResult::Invalid;
			}
			if (AxisConfig.AxisMinValue > AxisConfig.AxisMaxValue)
			{
				Context.AddError(FText::FromString(TEXT("AxisMaxValue must be greater than or equal to AxisMinValue")));
				Result = EDataValidationResult::Invalid;
			}
		}
	}
	
	return Result == EDataValidationResult::Invalid	? EDataValidationResult::Invalid : EDataValidationResult::Valid;
}
#endif



void UBattleUtilityAIData::ApplyAxisFunctionPreset(FAxisConfig& OutAxisConfig)
{
	switch (OutAxisConfig.FunctionPreset)
	{
	case EAxisFunctionPreset::Custom:
		return;

	/*
	 * Quadratic
	 * Result = Slope * Pow(X - HorizontalShift, Exponent) + VerticalShift
	 */

	case EAxisFunctionPreset::LinearHigh:
		OutAxisConfig.FunctionType = EAxisFunctionType::Quadratic;
		OutAxisConfig.Slope = 1.0f;
		OutAxisConfig.Exponent = 1.0f;
		OutAxisConfig.VerticalShift = 0.0f;
		OutAxisConfig.HorizontalShift = 0.0f;
		break;

	case EAxisFunctionPreset::LinearLow:
		OutAxisConfig.FunctionType = EAxisFunctionType::Quadratic;
		OutAxisConfig.Slope = -1.0f;
		OutAxisConfig.Exponent = 1.0f;
		OutAxisConfig.VerticalShift = 1.0f;
		OutAxisConfig.HorizontalShift = 0.0f;
		break;

	case EAxisFunctionPreset::EaseInHigh:
		// y = x^2
		// 낮을 때 거의 0, 높아질수록 빠르게 상승
		OutAxisConfig.FunctionType = EAxisFunctionType::Quadratic;
		OutAxisConfig.Slope = 1.0f;
		OutAxisConfig.Exponent = 2.0f;
		OutAxisConfig.VerticalShift = 0.0f;
		OutAxisConfig.HorizontalShift = 0.0f;
		break;

	case EAxisFunctionPreset::HardLateHigh:
		// y = x^4
		// 거의 1에 가까울 때만 높음
		OutAxisConfig.FunctionType = EAxisFunctionType::Quadratic;
		OutAxisConfig.Slope = 1.0f;
		OutAxisConfig.Exponent = 4.0f;
		OutAxisConfig.VerticalShift = 0.0f;
		OutAxisConfig.HorizontalShift = 0.0f;
		break;

	case EAxisFunctionPreset::EaseOutHigh:
		// y = 1 - (x - 1)^2
		// 조금만 올라가도 빠르게 좋아짐
		OutAxisConfig.FunctionType = EAxisFunctionType::Quadratic;
		OutAxisConfig.Slope = -1.0f;
		OutAxisConfig.Exponent = 2.0f;
		OutAxisConfig.VerticalShift = 1.0f;
		OutAxisConfig.HorizontalShift = 1.0f;
		break;

	case EAxisFunctionPreset::HardEarlyHigh:
		// y = 1 - (x - 1)^4
		// 초반부터 급격히 좋아짐
		OutAxisConfig.FunctionType = EAxisFunctionType::Quadratic;
		OutAxisConfig.Slope = -1.0f;
		OutAxisConfig.Exponent = 4.0f;
		OutAxisConfig.VerticalShift = 1.0f;
		OutAxisConfig.HorizontalShift = 1.0f;
		break;

	case EAxisFunctionPreset::EaseInLow:
		// y = (x - 1)^2
		// 초반에 빠르게 감소하고 후반에는 완만하게 감소
		OutAxisConfig.FunctionType = EAxisFunctionType::Quadratic;
		OutAxisConfig.Slope = 1.0f;
		OutAxisConfig.Exponent = 2.0f;
		OutAxisConfig.VerticalShift = 0.0f;
		OutAxisConfig.HorizontalShift = 1.0f;
		break;

	case EAxisFunctionPreset::EaseOutLow:
		// y = 1 - x^2
		// 초반에는 완만하게 감소하고 후반에는 빠르게 감소
		OutAxisConfig.FunctionType = EAxisFunctionType::Quadratic;
		OutAxisConfig.Slope = -1.0f;
		OutAxisConfig.Exponent = 2.0f;
		OutAxisConfig.VerticalShift = 1.0f;
		OutAxisConfig.HorizontalShift = 0.0f;
		break;

	case EAxisFunctionPreset::HardLateLow:
		// y = 1 - x^4
		// 0~0.6 정도까지는 높고, 후반에 급락
		OutAxisConfig.FunctionType = EAxisFunctionType::Quadratic;
		OutAxisConfig.Slope = -1.0f;
		OutAxisConfig.Exponent = 4.0f;
		OutAxisConfig.VerticalShift = 1.0f;
		OutAxisConfig.HorizontalShift = 0.0f;
		break;

	case EAxisFunctionPreset::PeakNear:
		// y = -16 * (x - 0.25)^2 + 1
		// 0.25 근처 선호
		OutAxisConfig.FunctionType = EAxisFunctionType::Quadratic;
		OutAxisConfig.Slope = -16.0f;
		OutAxisConfig.Exponent = 2.0f;
		OutAxisConfig.VerticalShift = 1.0f;
		OutAxisConfig.HorizontalShift = 0.25f;
		break;

	case EAxisFunctionPreset::PeakMid:
		// y = -4 * (x - 0.5)^2 + 1
		// 0.5 근처 선호
		OutAxisConfig.FunctionType = EAxisFunctionType::Quadratic;
		OutAxisConfig.Slope = -4.0f;
		OutAxisConfig.Exponent = 2.0f;
		OutAxisConfig.VerticalShift = 1.0f;
		OutAxisConfig.HorizontalShift = 0.5f;
		break;

	case EAxisFunctionPreset::PeakFar:
		// y = -16 * (x - 0.75)^2 + 1
		// 0.75 근처 선호
		OutAxisConfig.FunctionType = EAxisFunctionType::Quadratic;
		OutAxisConfig.Slope = -16.0f;
		OutAxisConfig.Exponent = 2.0f;
		OutAxisConfig.VerticalShift = 1.0f;
		OutAxisConfig.HorizontalShift = 0.75f;
		break;

	case EAxisFunctionPreset::ValleyMid:
		// y = 4 * (x - 0.5)^2
		// 0.5 근처가 나쁘고 양끝이 좋음
		OutAxisConfig.FunctionType = EAxisFunctionType::Quadratic;
		OutAxisConfig.Slope = 4.0f;
		OutAxisConfig.Exponent = 2.0f;
		OutAxisConfig.VerticalShift = 0.0f;
		OutAxisConfig.HorizontalShift = 0.5f;
		break;

	/*
	 * Logistic
	 * Result = Exponent / (1 + Exp(-Slope * (X - HorizontalShift))) + VerticalShift
	 */

	case EAxisFunctionPreset::SoftThresholdHigh:
		// 0.5 근처부터 부드럽게 상승
		OutAxisConfig.FunctionType = EAxisFunctionType::Logistic;
		OutAxisConfig.Slope = 8.0f;
		OutAxisConfig.Exponent = 1.0f;
		OutAxisConfig.VerticalShift = 0.0f;
		OutAxisConfig.HorizontalShift = 0.5f;
		break;

	case EAxisFunctionPreset::HardThresholdHigh:
		// 0.5 넘으면 급격히 상승
		OutAxisConfig.FunctionType = EAxisFunctionType::Logistic;
		OutAxisConfig.Slope = 16.0f;
		OutAxisConfig.Exponent = 1.0f;
		OutAxisConfig.VerticalShift = 0.0f;
		OutAxisConfig.HorizontalShift = 0.5f;
		break;

	case EAxisFunctionPreset::EarlyThresholdHigh:
		// 0.3 정도만 넘어도 좋음
		OutAxisConfig.FunctionType = EAxisFunctionType::Logistic;
		OutAxisConfig.Slope = 16.0f;
		OutAxisConfig.Exponent = 1.0f;
		OutAxisConfig.VerticalShift = 0.0f;
		OutAxisConfig.HorizontalShift = 0.3f;
		break;

	case EAxisFunctionPreset::LateThresholdHigh:
		// 0.7 이상부터 좋음
		OutAxisConfig.FunctionType = EAxisFunctionType::Logistic;
		OutAxisConfig.Slope = 16.0f;
		OutAxisConfig.Exponent = 1.0f;
		OutAxisConfig.VerticalShift = 0.0f;
		OutAxisConfig.HorizontalShift = 0.7f;
		break;

	case EAxisFunctionPreset::VeryLateThresholdHigh:
		// 0.8 이상부터 강하게 좋음
		OutAxisConfig.FunctionType = EAxisFunctionType::Logistic;
		OutAxisConfig.Slope = 20.0f;
		OutAxisConfig.Exponent = 1.0f;
		OutAxisConfig.VerticalShift = 0.0f;
		OutAxisConfig.HorizontalShift = 0.8f;
		break;

	case EAxisFunctionPreset::SoftThresholdLow:
		// 0.5 근처부터 부드럽게 감소
		OutAxisConfig.FunctionType = EAxisFunctionType::Logistic;
		OutAxisConfig.Slope = -8.0f;
		OutAxisConfig.Exponent = 1.0f;
		OutAxisConfig.VerticalShift = 0.0f;
		OutAxisConfig.HorizontalShift = 0.5f;
		break;

	case EAxisFunctionPreset::HardThresholdLow:
		// 0.5 넘으면 급격히 나빠짐
		OutAxisConfig.FunctionType = EAxisFunctionType::Logistic;
		OutAxisConfig.Slope = -16.0f;
		OutAxisConfig.Exponent = 1.0f;
		OutAxisConfig.VerticalShift = 0.0f;
		OutAxisConfig.HorizontalShift = 0.5f;
		break;

	case EAxisFunctionPreset::EarlyDropLow:
		// 0.3 넘으면 거의 싫음
		OutAxisConfig.FunctionType = EAxisFunctionType::Logistic;
		OutAxisConfig.Slope = -16.0f;
		OutAxisConfig.Exponent = 1.0f;
		OutAxisConfig.VerticalShift = 0.0f;
		OutAxisConfig.HorizontalShift = 0.3f;
		break;

	case EAxisFunctionPreset::LateDropLow:
		// 0.7 전까지는 괜찮고 이후 급락
		OutAxisConfig.FunctionType = EAxisFunctionType::Logistic;
		OutAxisConfig.Slope = -16.0f;
		OutAxisConfig.Exponent = 1.0f;
		OutAxisConfig.VerticalShift = 0.0f;
		OutAxisConfig.HorizontalShift = 0.7f;
		break;

	/*
	 * Logit
	 * ShiftedX = X - HorizontalShift
	 * Result = Slope * Loge(ShiftedX / (Exponent - ShiftedX)) + VerticalShift
	 *
	 * 보통 Exponent = 1.0f, HorizontalShift = 0.0f, VerticalShift = 0.5f
	 */

	case EAxisFunctionPreset::LogitEndHighSoft:
		// 0에 가까울수록 빠르게 값이 작아지고, 1에 가까울수록 빠르게 값이 커짐, 전체적으로 0.5 근처값
		OutAxisConfig.FunctionType = EAxisFunctionType::Logit;
		OutAxisConfig.Slope = 0.11f;
		OutAxisConfig.Exponent = 1.0f;
		OutAxisConfig.VerticalShift = 0.5f;
		OutAxisConfig.HorizontalShift = 0.0f;
		break;

	case EAxisFunctionPreset::LogitEndHighStrong:
		// LogitEndHighSoft보다 변동이 더 강하게 일어남, 양 끝단은 각 0과 1에 급격히 가까워지고, 전체적으로 0.4~0.6 근처값 
		OutAxisConfig.FunctionType = EAxisFunctionType::Logit;
		OutAxisConfig.Slope = 0.18f;
		OutAxisConfig.Exponent = 1.0f;
		OutAxisConfig.VerticalShift = 0.5f;
		OutAxisConfig.HorizontalShift = 0.0f;
		break;

	case EAxisFunctionPreset::LogitEndHighVeryStrong:
		// LogitEndHighStrong보다 변동이 더 강하게 일어남, 양 끝단은 각 0과 1에 급격히 가까워지고, 전체적으로 0.3~0.7 근처값
		OutAxisConfig.FunctionType = EAxisFunctionType::Logit;
		OutAxisConfig.Slope = 0.24f;
		OutAxisConfig.Exponent = 1.0f;
		OutAxisConfig.VerticalShift = 0.5f;
		OutAxisConfig.HorizontalShift = 0.0f;
		break;

	case EAxisFunctionPreset::LogitEndLowSoft:
		// 0에 가까울수록 빠르게 값이 커지고, 1에 가까울수록 빠르게 값이 작아짐, 전체적으로 0.5 근처값
		OutAxisConfig.FunctionType = EAxisFunctionType::Logit;
		OutAxisConfig.Slope = -0.11f;
		OutAxisConfig.Exponent = 1.0f;
		OutAxisConfig.VerticalShift = 0.5f;
		OutAxisConfig.HorizontalShift = 0.0f;
		break;

	case EAxisFunctionPreset::LogitEndLowStrong:
		// LogitEndLowSoft보다 변동이 더 강하게 일어남, 양 끝단은 각 1과 0에 급격히 가까워지고, 전체적으로 0.4~0.6 근처값
		OutAxisConfig.FunctionType = EAxisFunctionType::Logit;
		OutAxisConfig.Slope = -0.18f;
		OutAxisConfig.Exponent = 1.0f;
		OutAxisConfig.VerticalShift = 0.5f;
		OutAxisConfig.HorizontalShift = 0.0f;
		break;

	case EAxisFunctionPreset::LogitEndLowVeryStrong:
		// LogitEndLowStrong보다 변동이 더 강하게 일어남, 양 끝단은 각 1과 0에 급격히 가까워지고, 전체적으로 0.3~0.7 근처값
		OutAxisConfig.FunctionType = EAxisFunctionType::Logit;
		OutAxisConfig.Slope = -0.24f;
		OutAxisConfig.Exponent = 1.0f;
		OutAxisConfig.VerticalShift = 0.5f;
		OutAxisConfig.HorizontalShift = 0.0f;
		break;

	default:
		break;
	}
	
}
