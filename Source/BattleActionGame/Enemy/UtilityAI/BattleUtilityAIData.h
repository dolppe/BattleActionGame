#pragma once

#include "Engine/DataAsset.h"
#include "BattleUtilityAIData.generated.h"

enum class EDistanceSourceType : uint8;
enum class EAxisFunctionType : uint8;
enum class EBattleConsiderType : uint8;
class UBattleUtilityAction;


// 사용할 함수 프리셋
UENUM(BlueprintType)
enum class EAxisFunctionPreset : uint8
{
	Custom,

	// Quadratic
	LinearHigh UMETA(ToolTip = "선형 함수 0이면 0, 1이면 1"),
	LinearLow UMETA(ToolTip = "선형 함수 0이면 1, 1이면 0"),
	EaseInHigh UMETA(ToolTip = "y = x^2 / 1에 다가갈수록 빠르게 값이 커짐"),
	HardLateHigh  UMETA(ToolTip = "y = x^4 / 1에 가까울 때만 값이 커짐"),
	EaseOutHigh UMETA(ToolTip = "조금만 올라가도 빠르게 값이 커짐"),
	HardEarlyHigh  UMETA(ToolTip = "초반부터 급격히 값이 커짐"),
	EaseInLow  UMETA(ToolTip = "초반에 빠르게 감소하고 후반에는 완만하게 감소"),
	EaseOutLow  UMETA(ToolTip = "초반에는 완만하게 감소하고 후반에는 빠르게 감소"),
	HardLateLow  UMETA(ToolTip = "0~0.6까지는 값이 크지만, 그 뒤로는 빠르게 값이 작아짐"),
	PeakNear  UMETA(ToolTip = "0.25 근처에만 값이 큼"),
	PeakMid  UMETA(ToolTip = "0.5 근처에만 값이 큼"),
	PeakFar  UMETA(ToolTip = "0.75 근처에만 값이 큼"),
	ValleyMid  UMETA(ToolTip = "0.5 근처가 값이 작고, 양 끝단이 값이 큼"),

	// Logistic
	SoftThresholdHigh  UMETA(ToolTip = "0.5 근처부터 부드럽게 값이 커짐"),
	HardThresholdHigh  UMETA(ToolTip = "0.5를 넘으면 빠르게 값이 커짐"),
	EarlyThresholdHigh  UMETA(ToolTip = "0.3이상부터 값이 큼"),
	LateThresholdHigh  UMETA(ToolTip = "0.7이상부터 값이 큼"),
	VeryLateThresholdHigh  UMETA(ToolTip = "0.8이상부터 값이 큼"),
	SoftThresholdLow  UMETA(ToolTip = "0.5이상부터 부드럽게 값이 작아짐"),
	HardThresholdLow  UMETA(ToolTip = "0.5이상부터 빠르게 값이 작아짐"),
	EarlyDropLow  UMETA(ToolTip = "0.3이상부터 값이 매우 작음"),
	LateDropLow  UMETA(ToolTip = "0.7이상부터 값이 매우 작음"),

	// Logit
	LogitEndHighSoft  UMETA(ToolTip = "0에 가까울수록 빠르게 값이 작아지고, 1에 가까울수록 빠르게 값이 커짐, 전체적으로 0.5 근처값"),
	LogitEndHighStrong UMETA(ToolTip = "LogitEndHighSoft보다 변동이 더 강하게 일어남, 양 끝단은 각 0과 1에 급격히 가까워지고, 전체적으로 0.4~0.6 근처값"),
	LogitEndHighVeryStrong UMETA(ToolTip = "LogitEndHighStrong보다 변동이 더 강하게 일어남, 양 끝단은 각 0과 1에 급격히 가까워지고, 전체적으로 0.3~0.7 근처값"),
	LogitEndLowSoft UMETA(ToolTip = "0에 가까울수록 빠르게 값이 커지고, 1에 가까울수록 빠르게 값이 작아짐, 전체적으로 0.5 근처값"),
	LogitEndLowStrong UMETA(ToolTip = "LogitEndLowSoft보다 변동이 더 강하게 일어남, 양 끝단은 각 1과 0에 급격히 가까워지고, 전체적으로 0.4~0.6 근처값"),
	LogitEndLowVeryStrong UMETA(ToolTip = "LogitEndLowStrong보다 변동이 더 강하게 일어남, 양 끝단은 각 1과 0에 급격히 가까워지고, 전체적으로 0.3~0.7 근처값")
	
};


// Axis에 대한 정보를 담음
USTRUCT(BlueprintType)
struct FAxisConfig
{
	GENERATED_BODY()
	
	FAxisConfig();
	
	// 평가 항목 타입
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Axis")
	EBattleConsiderType ConsiderType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Axis")
	EAxisFunctionPreset FunctionPreset = EAxisFunctionPreset::Custom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Axis")
	EAxisFunctionType FunctionType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Axis")
	float Slope = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Axis")
	float Exponent = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Axis")
	float VerticalShift = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Axis")
	float HorizontalShift = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Axis")
	float AxisMinValue = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Axis")
	float AxisMaxValue = 1.0f;
	
};

// Action에 대한 정보를 담음
USTRUCT(BlueprintType)
struct FActionConfig
{
	GENERATED_BODY()
	
	FActionConfig();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action")
	TSubclassOf<UBattleUtilityAction> ActionClass;

	// Action에 설정할 Axis 목록
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action")
	TArray<FAxisConfig> AxisConfigs;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action")
	float Weight = 1.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action")
	float CoolTime = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action")
	float AgeTime = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action")
	float AgeRate = 1.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action")
	EDistanceSourceType DistanceSourceType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action")
	float MinDistance = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action")
	float MaxDistance = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Action")
	bool bIsActive = true;
	
};


/* UtilityAI에서 사용하는 Action, Axis에 대한 정보를 관리
 *	함수 프리셋을 지정하면 설정된 값이 적용되도록 했음.
 */

UCLASS(BlueprintType)
class UBattleUtilityAIData : public UDataAsset
{
	GENERATED_BODY()
public:
	
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) override;
#endif

	// 수집하고 계산할 평가 항목 목록
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UtilityAI")
	TArray<EBattleConsiderType> Consider;
	
	// 사용할 Action 목록
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UtilityAI")
	TArray<FActionConfig> ActionConfigs;
	
	// 함수 프리셋에 따라 함수 종류와 계수를 설정함
	static void ApplyAxisFunctionPreset(FAxisConfig& OutAxisConfig);
	
#if WITH_EDITOR
	virtual void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent) override;
#endif


};

