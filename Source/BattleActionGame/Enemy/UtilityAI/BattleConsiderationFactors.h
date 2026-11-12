#pragma once

#include "UObject/Object.h"
#include "BattleConsiderationFactors.generated.h"

class UBattleHealthComponent;
class ABattleCharacterBase;
class UBattleUtilityAIData;
enum class EAxisType : uint8;

/*
 *	상황 정보를 토대로 하는 평가 항목의 타입
 */

UENUM(BlueprintType)
enum class EBattleConsiderType : uint8
{
	/* 
	 * Single - Simple
	 */
	
	// [0 ~ 1] 0 => Hp가 0, 1 => Hp가 최대치
	MyHp UMETA(DisplayName = "MyHp"),
	// [0 or 1] 0 => 선택된 Target이 없음, 1 => 선택된 Target이 존재
	HasTarget UMETA(DisplayName = "HasTarget"),
	// [0 or 1] 0 => 선택된 Target이 왼쪽에 있음, 1 => 선택된 Target이 오른쪽에 있음
	TargetHorizontalDirection UMETA(DisplayName = "TargetHorizontalDirection"),
	// [0 or 1] 0 => 선택된 Target이 후방에 있음, 1 => 선택된 Target이 전방에 있음
	TargetForwardDirection UMETA(DisplayName = "TargetForwardDirection"),
	// [0 ~ 1] 0 => 선택된 Target이 바로 근처에 있음, 1 => 선택된 Target이 먼 거리에 있음 
	IsFarFromTarget UMETA(DisplayName = "IsFarFromTarget"),
	// [0 ~ 1] 0 => 선택된 Target과 나 사이의 장애물이 있음, 1 => 선택된 Target과 나 사이에 장애물이 없음. 
	TargetSightClear UMETA(DisplayName = "TargetSightClear"),
	// [0 ~ 1] 0 =>  주변에 가까운 적이 없음, 1 => 주변에 가까운 적이 많음
	NearbyEnemyPressure UMETA(DisplayName = "NearbyEnemyPressure"),
	// [0 ~ 1] 0 => 비전투, 0.5 => 적정 전투 시간, 1 => 너무 오랫동안 전투 중
	CombatDuration UMETA(DisplayName = "CombatDuration"),
	// [0 or 1] 0 => 비전투 중, 1 => 전투 중
	IsInCombat UMETA(DisplayName = "IsInCombat"),
	// [0 or 1] 0 => 움직일 수 없음, 1 => 움직일 수 있음
	CanMove UMETA(DisplayName = "CanMove"),
	// [0 or 1] 0 => 공격할 수 없음, 1 => 공격할 수 있음
	CanAttack UMETA(DisplayName = "CanAttack"),
	// [0 ~ 1] 0 => 적들이 밀집해 있음, 1 => 적들이 흩어져 있음
	EnemySpread UMETA(DisplayName = "EnemySpread"),
	// [0 ~ 1] 0 => 적들과의 평균 거리가 가까움, 1 => 적들과의 평균 거리가 멈
	EnemyAverageDistance UMETA(DisplayName = "EnemyAverageDistance"),
	// [0 or 1] 0 => 주변에 Cave가 없음, 1 => 주변에 Cave가 있음
	NearByCave UMETA(DisplayName = "NearByCave"),
	// [0 or 1] 0 => 주변에 Desert가 없음, 1 => 주변에 Desert가 있음
	NearByDesert UMETA(DisplayName = "NearByDesert"),
	// [0 or 1] 0 => 주변에 Ruin이 없음, 1 => 주변에 Ruin이 있음
	NearByRuin UMETA(DisplayName = "NearByRuin"),
	// [0 or 1] 0 => 주변에 좋은 Spot이 없음, 1 => 주변에 좋은 Spot이 있음 
	NearByBestCombatSpot UMETA(DisplayName = "NearByBestCombatSpot"),
	// [0 ~ 1] 0 => 적들에게 둘러 쌓여있지 않음, 1 => 적들에게 사방으로 둘러 쌓여있음
	SurroundedRisk UMETA(DisplayName = "SurroundedRisk"),
	// [0 or 1] 0 => 현재 자리가 좋음, 1 => 현재 자리가 별로임
	PositionalDisadvantage UMETA(DisplayName = "PositionalDisadvantage"),
	
	// 미구현 => 내가 독을 얼마나 가지고 있는지
	PoisonAmount UMETA(DisplayName = "PoisonAmount"),
	// 미구현 => 내가 전기를 얼마나 가지고 있는지
	ElectricityAmount UMETA(DisplayName = "ElectricityAmount"),
	
	/* 
	 * Single - Complex
	 */
	
	// [0 ~ 1] 0 => 전투할 여력이 없음, 1 => 만전의 상태
	// 전투 가능한 상태인지를 나타냄
	MyCombatPotential UMETA(DisplayName = "MyCombatPotential"),
	// [0 ~ 1] 0 => 위협이 전혀 없음, 1 => 많이 위험한 상태
	// 내가 받고 있는 위협 정도를 의미
	ThreatScore UMETA(DisplayName = "ThreatScore"),
	// [0 ~ 1] 0 => 현재 자리가 가장 좋음, 1 => 현재 자리가 매우 안좋음
	// 전투 자리 재조정이 필요한지를 의미
	NeedCombatReposition UMETA(DisplayName = "NeedCombatReposition"),
	// [0 ~ 1] 0 => 지역을 이동할 필요가 없음, 1 => 지역 이동을 해야함
	// 지역 이동이 필요한지를 의미
	NeedCombatAreaChange UMETA(DisplayName = "NeedCombatAreaChange"),
	
	/* 
	 * Multi - Target - Simple
	 * 
	 * Target들에 대한 값들을 Array로 가짐.
	 * Action쪽에서 Axis 함수와 합쳐져서 나온 점수들 중 가장 높은 점수를 가진 Target을 택함 
	 */

	// [0 ~ 1] 0 => 타겟과의 거리가 가까움, 1 => 타겟과의 거리가 멈 
	TargetDistance UMETA(DisplayName = "TargetDistance"),
	// [0 ~ 1] 0 => 타겟의 Hp가 낮음, 1 => 타겟의 Hp가 높음
	TargetHp UMETA(DisplayName = "TargetHp"),
	// [0 ~ 1] 0 => 독에 안걸린 상태, 1 => 독에 걸린 상태
	TargetPoisonedState UMETA(DisplayName = "TargetPoisonedState"),
	
	/* 
	* Multi - Target - Complex
	*/
	
	// [0 ~ 1] 0 => 타겟의 우선순위가 낮음, 1 => Target의 우선순위가 높음
	// 타겟들에 대한 우선순위를 나타냄
	TargetPriority UMETA(DisplayName = "TargetPriority"),
	// [0 ~ 1] 0 => 타겟이 건강함, 1 => 타겟이 약해져있음
	// 타겟들의 상태가 어떤지를 나타냄
	TargetWeakness UMETA(DisplayName = "TargetWeakness"),
	
	
	
};


// ConsiderationFactors에 사용될 설정값
USTRUCT(BlueprintType)
struct FConsiderationSettings
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	float MaxTargetDistance = 5000.0f;
	UPROPERTY(EditAnywhere)
	float ActorSearchRadius = 5000.f;
	UPROPERTY(EditAnywhere)
	FVector SpotSearchExtent = FVector(3000.f, 3000.f, 6000.f);
	
	
	UPROPERTY(EditAnywhere)
	float BestCombatTime = 30.0f;
	UPROPERTY(EditAnywhere)
	float ThreatCharacterNum = 3.0f;
};


/*
 *	AI 평가에 필요한 상황 정보를 수집, 설정된 평가 항목의 값들을 계산 및 저장.
 *	저장된 평가값은 여러 Action에서 공통으로 사용.
 */

UCLASS(BlueprintType)
class UConsiderationFactors : public UObject
{
	GENERATED_BODY()
	
public:
	
	UConsiderationFactors();

	void InitConsiderFunction(const UBattleUtilityAIData* InUtilityAIData, AActor* InOwner, const FConsiderationSettings& InSettings);
	
	// 주변 상황 정보를 수집
	void CollectConsiderationData();
	// 수집한 상황 정보를 바탕으로 등록된 평가값을 계산 및 저장
	void CalcConsiderationFactors();
	
	// 계산된 평가값 조회
	float GetFactorValue(EBattleConsiderType ConsiderType) const;
	const TArray<float>* GetMultiFactorValues(EBattleConsiderType ConsiderType) const;
	
	ABattleCharacterBase* GetSelectedTarget() const
	{
		return SelectedTarget.Get();
	}
	
	void SetSelectedTarget(ABattleCharacterBase* InSelectedTarget);

	
	// 평가 항목이 사용하는 단일, 후보별 평가 유형을 반환
	static EAxisType GetAxisType(EBattleConsiderType ConsiderType);
	
	// 평가 유형과 인덱스에 해당하는 후보 반환
	AActor* GetCandidatePtr(EAxisType InAxisType, int32 Index) const;
	
	// 평가 유형에 해당하는 배열의 Size를 반환
	int32 GetCandidateArrNum(EAxisType InAxisType) const;

	ABattleCharacterBase* GetMyCharacter()
	{
		return MyCharacter;
	}
	
	const FVector& GetBestSpotLocation() const
	{
		return BestSpotLocation;
	}
	
protected:
	
	// 평가값 계산 함수와 평가값 저장.
	TMap<EBattleConsiderType, float> Factors;
	TMap<EBattleConsiderType, TFunction<float()>> FactorCalculators;

	TMap<EBattleConsiderType, TArray<float>> MultiFactors;
	TMap<EBattleConsiderType, TFunction<TArray<float>()>> MultiFactorCalculators;

private:
	
	/*
	 * 상황 정보를 알아오는 함수
	 */
	void SearchNearSpots();
	void SearchNearActors();
	void ClearNearActorsData();
	
	// 데미지 이벤트 처리
	UFUNCTION()
	void OnCharacterHealthChanged(UBattleHealthComponent* HealthComponent, float OldValue, float NewValue, AActor* Instigator);
	
	/*
	 * 평가 항목과 평가값 계산 함수 연결
	 */
	TFunction<float()> GetConsiderFunction(EBattleConsiderType ConsiderType) const;
	TFunction<TArray<float>()> GetMultiConsiderFunction(EBattleConsiderType ConsiderType) const;

	// 후보별 평가값 계산 함수
	TArray<float> GetTargetDistance() const;
	TArray<float> GetTargetHp() const;
	TArray<float> GetTargetPriority() const;
	TArray<float> GetTargetWeakness() const;
	TArray<float> GetTargetPoisonedState() const;

	// 단일 평가값 계산 함수
	float GetMyHp() const;
	float GetHasTarget() const;
	float GetMyCombatPotential() const;
	float GetTargetHorizontalDirection() const;
	float GetTargetForwardDirection() const;
	float GetNearbyEnemyPressure() const;
	float GetThreatScore() const;
	float GetCombatDuration() const;
	float GetIsInCombat() const;
	float GetCanMove() const;
	float GetEnemySpread() const;
	float GetEnemyAverageDistance() const;
	float GetIsFarFromTarget() const;
	float GetCanAttack() const;
	float GetTargetSightClear() const;
	float GetPoisonAmount() const;
	float GetElectricityAmount() const;
	float GetNearByCave() const;
	float GetNearByDesert() const;
	float GetNearByRuin() const;
	float GetNearByBestCombatSpot() const;
	float GetNeedCombatAreaChange() const;
	float GetNeedCombatReposition() const;
	float GetSurroundedRisk() const;
	float GetPositionalDisadvantage() const;

private:
	
	// OwnerCharacter
	UPROPERTY()
	TObjectPtr<ABattleCharacterBase> MyCharacter;
	
	// 상황 정보 수집 관련 세팅값
	FConsiderationSettings Settings;

private:
	
	/*
	 *	주변 정보 - 기본
	 */
	
	float CurTime = 0.f;
	float CombatStartTime = -1;
	bool bIsInCombat = false;	
	
	float MyHp;
	float PoisonAmount = 1.0f;
	float ElectricAmount = 1.0f;
	
	TMap<TWeakObjectPtr<ABattleCharacterBase>, float> DamageStack;
	float TotalDamage = 0.0f;
	
	/*
	 *	주변 정보 - Actor 관련
	 */
	
	TWeakObjectPtr<ABattleCharacterBase> SelectedTarget;
	
	// 주변 후보와 후보별 수집값은 같은 인덱스에 같은 후보에 대한 정보가 존재
	TArray<TWeakObjectPtr<ABattleCharacterBase>> TargetActors;
	TArray<float> TargetDistances;
	TArray<float> TargetHps;
	
	float EnemySpread;
	float EnemyAverageDistance;
	float SelectedTargetDistance;
	bool bIsSelectedTargetSightClear = false;
		
	
	/*
	 *	주변 정보 - Spot 관련
	 */
	int32 TotalAreaFlags = 0;
	int32 MyCharacterAreaFlag = 0;
	FVector BestSpotLocation;
	
};

