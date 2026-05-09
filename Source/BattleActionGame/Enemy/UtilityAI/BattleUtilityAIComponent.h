#pragma once

#include "Components/PawnComponent.h"
#include "VisualLogger/VisualLoggerTypes.h"
#include "BattleUtilityAIComponent.generated.h"


struct FGameplayTag;
class UBattleUtilityAIComponent;
class ABattleCharacterBase;
class UBattleUtilityAIData;
class UBattleUtilityAction;
class UBattleHealthComponent;

UENUM(BlueprintType)
enum class EBattleConsiderType : uint8
{
	/* 
	 * Single - Simple
	 */
	
	// [0 ~ 1] 0 => Hp가 0, 1 => Hp가 최대치
	MyHp UMETA(DisplayName = "MyHp"),
	// [0 or 1] 0 => Target이 없음, 1 => Target이 존재
	HasTarget UMETA(DisplayName = "HasTarget"),
	// [0 or 1] 0 => Target이 왼쪽에 있음, 1 => Target이 오른쪽에 있음
	TargetHorizontalDirection UMETA(DisplayName = "TargetHorizontalDirection"),
	// [0 or 1] 0 => Target이 후방에 있음, 1 => Target이 전방에 있음
	TargetForwardDirection UMETA(DisplayName = "TargetForwardDirection"),
	// [0 ~ 1] 0 => Target이 바로 근처에 있음, 1 => Target이 먼 거리에 있음 
	IsFarFromTarget UMETA(DisplayName = "IsFarFromTarget"),
	// [0 ~ 1] 0 => Target과 나 사이의 장애물이 있음, 1 => Target과 나 사이에 장애물이 없음. 
	TargetSightClear UMETA(DisplayName = "TargetSightClear"),
	// [0 ~ 1] 0 =>  주변에 가까운 적이 없음, 1 => 주변에 가까운 적이 많음
	NearbyEnemyPressure UMETA(DisplayName = "NearbyEnemyPressure"),
	// [0 ~ 1] 0 => 비전투, 0.5 => 적정 전투 시간, 1 => 너무 오랫동안 전투 중
	CombatDuration UMETA(DisplayName = "CombatDuration"),
	// [0 ~ 1] 0 => 왼쪽에 적이 더 많음, 1 => 오른쪽에 적이 더 많음
	EnemyDirection UMETA(DisplayName = "EnemyDirection"),
	// [0 or 1] 0 => 비전투 중, 1 => 전투 중
	IsInCombat UMETA(DisplayName = "IsInCombat"),
	// [0 or 1] 0 => 왼쪽 다리가 멀쩡 함, 1 => 왼쪽 다리가 파괴됨
	BreakLeftLeg UMETA(DisplayName = "BreakLeftLeg"),
	// [0 or 1] 0 => 오른쪽 다리가 멀쩡 함, 1 => 오른쪽 다리가 파괴됨
	BreakRightLeg UMETA(DisplayName = "BreakRightLeg"),
	// [0 or 1] 0 => 움직일 수 없음, 1 => 움직일 수 있음
	CanMove UMETA(DisplayName = "CanMove"),
	// [0 or 1] 0 => 공격할 수 없음, 1 => 공격할 수 있음
	CanAttack UMETA(DisplayName = "CanAttack"),
	// [0 ~ 1] 0 => 적들이 밀집해 있음, 1 => 적들이 흩어져 있음
	EnemySpread UMETA(DisplayName = "EnemySpread"),
	// [0 ~ 1] 0 => 적들과의 평균 거리가 가까움, 1 => 적들과의 평균 거리가 멈
	EnemyAverageDistance UMETA(DisplayName = "EnemyAverageDistance"),
	// 미구현 => 주변에 물이 얼마나 있는지
	NearbyWaterAmount UMETA(DisplayName = "NearbyWaterAmount"),
	// 미구현 => 내가 독을 얼마나 가지고 있는지
	PoisonAmount UMETA(DisplayName = "PoisonAmount"),
	// 미구현 => 내가 전기를 얼마나 가지고 있는지
	ElectricityAmount UMETA(DisplayName = "ElectricityAmount"),
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
	// [0 or 1] 0 => 현재 자리가 좋음, 1 => 현재 자리가 구림 
	PositionalDisadvantage UMETA(DisplayName = "PositionalDisadvantage"),
	
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
	 * Array - Target - Simple
	 * 
	 * Target들에 대한 값들을 Array로 가짐.
	 * Action쪽에서 Evaluate 함수와 합쳐져서 나온 점수들 중 가장 높은 점수를 가진 Target을 택함 
	 * 해당 Target은 SelectedTarget에 저장됨. 즉, Action마다 선택되는 Target이 다른 것임
	 */

	// [0 ~ 1] 0 => 타겟과의 거리가 가까움, 1 => 타겟과의 거리가 멈 
	TargetDistance UMETA(DisplayName = "TargetDistance"),
	// [0 ~ 1] 0 => 타겟의 Hp가 낮음, 1 => 타겟의 Hp가 높음
	TargetHp UMETA(DisplayName = "TargetHp"),
	// [0 ~ 1] 0 => 독에 안걸린 상태, 1 => 독에 걸린 상태
	TargetPoisonedState UMETA(DisplayName = "TargetPoisonedState"),
	
	/* 
	* Array - Target - Complex
	*/
	
	// [0 ~ 1] 0 => 타겟의 우선순위가 낮음, 1 => Target의 우선순위가 높음
	// 타겟들에 대한 우선순위를 나타냄
	TargetPriority UMETA(DisplayName = "TargetPriority"),
	// [0 ~ 1] 0 => 타겟이 건강함, 1 => 타겟이 약해져있음
	// 타겟들의 상태가 어떤지를 나타냄
	TargetWeakness UMETA(DisplayName = "TargetWeakness"),
	
	
	
};

UENUM(BlueprintType)
enum class EAxisType : uint8
{
	Single,
	Target,
	None,
};

DECLARE_ENUM_TO_STRING(EBattleConsiderType);



USTRUCT(BlueprintType)
struct FArrayFactorData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	EAxisType AxisType;
	UPROPERTY(BlueprintReadWrite)
	TArray<float> ArrayFactorScoreOrigin;
	UPROPERTY(BlueprintReadWrite)
	TArray<float> ArrayFactorScoreFinal;
};

USTRUCT(BlueprintType)
struct FUtilityAIScoreData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	float ActionScore;

	// [Factor1] [Factor2]
	UPROPERTY(BlueprintReadWrite)
	TArray<float> NormalFactorScoreOrigin;
	UPROPERTY(BlueprintReadWrite)
	TArray<float> NormalFactorScoreFinal;
	UPROPERTY(BlueprintReadWrite)
	TArray<EBattleConsiderType> NormalFactorConsiderType;

	// [ArrayFactor1-Target1] [ArrayFactor1-Target2] [ArrayFactor2-Target1] ...
	// [Target1-TargetFactor1/TargetFactor2/TargetFactor3] [Target2-TargetFactor1/TargetFactor2/TargetFactor3] [Spot1-SpotFactor1/SpotFactor2] [Spot2-SpotFactor1/SpotFactor2]
	UPROPERTY(BlueprintReadWrite)
	TArray<FArrayFactorData> ArrayFactorScore;
	
};

USTRUCT(BlueprintType)
struct FUtilityAITotalData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	TArray<FUtilityAIScoreData> UtilityAIScoreData;

	UPROPERTY(BlueprintReadWrite)
	FString ActiveActionName;

	UPROPERTY(BlueprintReadWrite)
	float CurTime;	
};



//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUtilityAIScoreDelegate, const TArray<FUtilityAIScoreData>&,UtilityAIScoreDatas);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUtilityAIScoreDelegate, const FUtilityAITotalData&,UtilityAIScoreDatas);

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUtilityAIScoreDelegateTest, TArray<FUtilityAIScoreData>&,UtilityAIScoreDatas);

UCLASS(BlueprintType)
class UConsiderationFactors : public UObject
{
	GENERATED_BODY()

public:


	/*
	 * 상황 정보를 토대로 ConsiderFactor를 만드는 함수
	 */

	UConsiderationFactors();

	// ArrayAxis

	TArray<float> GetTargetDistance();
	TArray<float> GetTargetHp();
	TArray<float> GetTargetPriority();
	TArray<float> GetTargetWeakness();
	TArray<float> GetTargetPoisonedState();
	TArray<float> GetTargetAggroValue();

	// Axis
	float GetMyHp();
	float GetTarget();
	float GetMyCombatPotential();
	float GetTargetHorizontalDirection();
	float GetTargetForwardDirection();
	float GetNearbyEnemyPressure();
	float GetThreatScore();
	float GetCombatDuration();
	float GetNearbyWaterAmount();
	float GetEnemyDirection();
	float GetIsInCombat();
	float GetBreakRightLeg();
	float GetBreakLeftLeg();
	float GetCanMove();
	float GetEnemySpread();
	float GetEnemyAverageDistance();
	float GetIsFarFromTarget();
	float GetCanAttack();
	float GetTargetSightClear();

	float GetPoisonAmount();
	float GetElectricityAmount();
	float GetNearByCave();
	float GetNearByDesert();
	float GetNearByRuin();
	float GetNearByBestCombatSpot();
	float GetNeedCombatAreaChange();
	float GetNeedCombatReposition();
	float GetSurroundedRisk();
	float GetPositionalDisadvantage();

		

	TFunction<float()> GetConsiderFunction(EBattleConsiderType ConsiderType);
	TFunction<TArray<float>()> GetArrayConsiderFunction(EBattleConsiderType ConsiderType);

	EAxisType GetAxisType(EBattleConsiderType ConsiderType);
	
	void InitConsiderFunction(const UBattleUtilityAIData* UtilityAIData, UBattleUtilityAIComponent* InUtilityAIComponent);
	void ClearConsiderFactors();


	/*
	 * 상황 정보를 알아오는 함수
	 */

	void GetConsiderListData();
	void SearchNearSpots();
	void SearchNearActors();

	UFUNCTION()
	void OnCharacterHealthChanged(UBattleHealthComponent* HealthComponent, float OldValue, float NewValue, AActor* Instigator);

	AActor* GetTargetPtr(EAxisType InAxisType, int Index) const;
	
public:

	TMap<EBattleConsiderType, EAxisType> ConsiderTypeMap;
	
	TMap<EBattleConsiderType, float> Factors;
	TMap<EBattleConsiderType, TFunction<float()>> Functions;

	TMap<EBattleConsiderType, TArray<float>> ArrayFactors;
	TMap<EBattleConsiderType, TFunction<TArray<float>()>> ArrayFunctions;

	TObjectPtr<ABattleCharacterBase> SelectedTarget;
	TArray<TObjectPtr<ABattleCharacterBase>> TargetActors;
	TArray<float> TargetAggroValues;
	TArray<float> TargetDistances;
	TArray<float> TargetHps;
	
	TMap<TObjectPtr<ABattleCharacterBase>, float> DamageStack;
	float TotalDamage = 0.0f;
	const float MaxDamageForPriority = 100.0f; 

	float MyHp;
	
	bool bIsInCombat = false;
	
	float LastAttackTime = -1000.f;
	bool bAllowedAttackTime = false;
	const float AllowedAttackDuration = 1.0f;
	
	float CombatStartTime = -1;

	float BestCombatTime;
	float ThreatCharacterNum;

	float EnemySpread;
	float EnemyAverageDistance;

	float SelectedTargetDistance;
	bool bIsSelectedTargetSightClear = false;
	
	TObjectPtr<ABattleCharacterBase> MyCharacter;
	TObjectPtr<UBattleUtilityAIComponent> UtilityAIComponent;

	bool bBreakRightLeg = false;
	bool bBreakLeftLeg = false;

	uint16 TotalAreaFlags = 0;
	uint16 MyCharacterAreaFlag = 0;

	FVector BestSpotLocation;
	
};


UCLASS(Blueprintable, Meta=(BlueprintSpawnableComponent))
class UBattleUtilityAIComponent : public UPawnComponent
{
	GENERATED_BODY()
public:

	UBattleUtilityAIComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;

	virtual void PostInitProperties() override;

	UFUNCTION(BlueprintCallable)
	const UBattleUtilityAIData* GetUtilityAIData() const
	{
		return UtilityAIData;
	}

	void CollectConsiderFactors();
	
	void SelectBestAction();
	
	bool CheckActionDistance(UBattleUtilityAction* UtilityAction, float SelectedTargetDistance);

	void StartTimer();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void BreakParts(FGameplayTag GameplayTag);

	TArray<FUtilityAIScoreData>& GetUtilityAIScoreData()
	{
		return UtilityAIScoreDatas;
	}

	UFUNCTION(BlueprintCallable)
	void SetDebugActionsEnabled(int Index, bool bEnabled)
	{
		if (DebugActionsEnabled.IsValidIndex(Index))
		{
			DebugActionsEnabled[Index] = bEnabled;
		}
	}

	friend class UBattleUtilityAxis;
	friend class UBattleUtilityArrayAxis;
	friend class UBattleUtilityAction;
	friend class UConsiderationFactors;

	UPROPERTY()
	TObjectPtr<UConsiderationFactors> ConsiderList;

	UPROPERTY(BlueprintAssignable)
	FUtilityAIScoreDelegate OnScoreChanged;
	
protected:

	UPROPERTY(EditAnywhere)
	TObjectPtr<const UBattleUtilityAIData> UtilityAIData;

	UPROPERTY()
	TArray<TObjectPtr<UBattleUtilityAction>> InstancedActions;

	UPROPERTY()
	UBattleUtilityAction* ActiveAction;
	
	UPROPERTY()
	TArray<float> LastActiveTime;
	

	bool bActionComplete;

	FTimerHandle TimerHandle;

	

	// 특정 거리에 Character가 있는지 체크 => 있으면 계산 주기를 0.5, 없으면 1.0 => 3.0 => 5.0...
	// 캐릭터 발견 시 즉각 반응하도록
	UPROPERTY(EditAnywhere)
	float UpdatePeriod = 0.5f;
	UPROPERTY(EditAnywhere)
	float MaxTargetDistance = 5000.0f;
	UPROPERTY(EditAnywhere)
	float BestCombatTime = 10.0f;
	UPROPERTY(EditAnywhere)
	float ThreatCharacterNum = 3.0f;

	TArray<FUtilityAIScoreData> UtilityAIScoreDatas;
	
	
private:

	bool NeedDebug = true;

	TArray<bool> DebugActionsEnabled;

	
	
	
};

