#pragma once

#include "UObject/Object.h"
#include "BattleUtilityAction.generated.h"

enum class EAxisType : uint8;
struct FActionConfig;
class UConsiderationFactors;
class UBattleUtilityAxis;

namespace BattleUtilityActionHelper
{
	// SpotLists의 XY 점들을 포함하는 원의 중심을 그리디 방식으로 계산하여 반환
	TArray<FVector> FindBestSpotsWithGreedy(const TArray<FVector>& SpotLists, float AreaRadius, int32 AreaNum);
}

UENUM(BlueprintType)
enum class EDistanceSourceType : uint8
{
	None,
	SelectedTarget
};

/*
 *	Axis 점수를 결합하여 Action 점수 계산 및 유형별 최적 후보 계산
 *	Action의 실행, Tick 종료도 담당
 */

UCLASS(BlueprintType, Blueprintable)
class UBattleUtilityAction : public UObject
{
	GENERATED_BODY()
public:

	UBattleUtilityAction();

	// 기본값 세팅 및 사용할 Axis 생성
	void InitAction(const FActionConfig& ActionConfig, UConsiderationFactors* InConsiderationFactors);
	
	// Axis 점수를 결합하여 최종 Action 점수를 반환.
	// 유형별 최적 후보 갱신
	virtual float EvaluateScore();

	/*
	 * Action 실행 관련 함수
	 */
	virtual void StartAction();
	
	virtual void EndAction();
	
	virtual void TickAction(float DeltaTime);
	
	virtual void SetStartAction();
	
	bool IsCompleteAction() const
	{
		return bIsCompletedAction;
	}
	
	/*
	 *	Action 설정값 관련
	 */
	
	float GetCoolTime() const
	{
		return CoolTime;
	}
	
	float GetAgeTime() const
	{
		return AgeTime;
	}
	
	float GetAgeRate() const
	{
		return AgeRate;
	}
	
	// Distance 조건 체크
	bool CanAllowedRange(float InDistance) const;
	EDistanceSourceType GetDistanceSourceType() const
	{
		return DistanceSourceType;
	}

protected:

	// Instance화된 Axis 중 Single만 저장
	UPROPERTY()
	TArray<TObjectPtr<UBattleUtilityAxis>> SingleAxisList;

	// MultiAxis에 사용되는 AxisType 목록
	UPROPERTY()
	TSet<EAxisType> AxisTypes;

	// Instance화된 Axis 중 Multi만 저장
	UPROPERTY()
	TArray<TObjectPtr<UBattleUtilityAxis>> MultiAxisList;
	
	// MultiAxis에서 선택된 대상
	UPROPERTY()
	TMap<EAxisType, TObjectPtr<AActor>> BestTargets;
	
	bool bIsCompletedAction = false;
	
	/*
	 *	Action 세팅값
	 */
	
	float Weight = 1.f;
	float CoolTime = 0.0f;
	float AgeTime = 0.0f;
	float AgeRate = 1.0f;
	
	EDistanceSourceType DistanceSourceType;
	float MinDistance;
	float MaxDistance;
	
	TWeakObjectPtr<UConsiderationFactors> ConsiderationFactors;

};
