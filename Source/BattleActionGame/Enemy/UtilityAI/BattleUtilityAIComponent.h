#pragma once

#include "BattleConsiderationFactors.h"
#include "Components/PawnComponent.h"
#include "BattleUtilityAIComponent.generated.h"

enum class EDistanceSourceType : uint8;
class UConsiderationFactors;
class UBattleUtilityAIData;
class UBattleUtilityAction;

/*
 *	UtilityAI의 실행 흐름을 담당하는 클래스
 */

UCLASS(Blueprintable, Meta=(BlueprintSpawnableComponent))
class UBattleUtilityAIComponent : public UPawnComponent
{
	GENERATED_BODY()
public:
	
	explicit UBattleUtilityAIComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	// 사용할 Action과 Axis에 대한 설정 데이터 반환
	UFUNCTION(BlueprintCallable)
	const UBattleUtilityAIData* GetUtilityAIData() const
	{
		return UtilityAIData;
	}
	
	// 생성된 Action 객체 목록 반환
	const TArray<TObjectPtr<UBattleUtilityAction>>& GetInstancedActions() const
	{
		return InstancedActions;
	}
	
	// 각 Action별 마지막 활성화 시각 목록 반환
	const TArray<float>& GetLastActiveTimes() const
	{
		return LastActiveTimes;
	}
	
private:
	
	// ConsiderationFactors에게 상황 정보 수집 및 평가값 계산 요청
	void CollectConsiderFactors();
	
	// Action별 점수 계산 및 가장 높은 점수를 가진 Action 활성화
	void SelectBestAction();
	
	// 실행 조건에 맞는 최고 점수 Action의 인덱스를 반환
	int32 FindBestActionIndex();
	
	// Distance Source의 Location을 반환
	bool GetDistanceSourceLocation(EDistanceSourceType DistanceSourceType, FVector& OutLocation) const;
	
	// Action의 거리 조건 체크
	bool CheckActionDistance(UBattleUtilityAction* UtilityAction) const;

	
private:
	
	// 실행할 Action 및 Axis 관련 설정 데이터
	UPROPERTY(EditAnywhere)
	TObjectPtr<const UBattleUtilityAIData> UtilityAIData;
	
	// 상황 정보 수집에 대한 설정 데이터
	UPROPERTY(EditAnywhere)
	FConsiderationSettings Settings;
	
	// 상황 정보 수집과 공통 평가값 계산을 수행
	UPROPERTY()
	TObjectPtr<UConsiderationFactors> ConsiderList;
	
	// 현재 실행중인 행동.
	UPROPERTY()
	TObjectPtr<UBattleUtilityAction> ActiveAction = nullptr;
	
	// 설정 데이터를 바탕으로 생성한 Action 객체
	UPROPERTY()
	TArray<TObjectPtr<UBattleUtilityAction>> InstancedActions;
	
	// Action의 마지막 실행 시각.
	// InstancedActions와 같은 인덱스에 같은 Action에 대한 정보가 담김 
	UPROPERTY()
	TArray<float> LastActiveTimes;
		
};

