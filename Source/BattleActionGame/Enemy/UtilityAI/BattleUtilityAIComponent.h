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
	// Single
	MyHp UMETA(DisplayName = "MyHp"),
	HasTarget UMETA(DisplayName = "HasTarget"),
	MyCombatPotential UMETA(DisplayName = "MyCombatPotential"),
	IsTargetInSight UMETA(DisplayName = "IsTargetInSight"),
	NearbyEnemyCount UMETA(DisplayName = "NearbyEnemyCount"),
	ThreatScore UMETA(DisplayName = "ThreatScore"),
	CombatDuration UMETA(DisplayName = "CombatDuration"),
	NearbyWaterAmount UMETA(DisplayName = "NearbyWaterAmount"),
	EnemyDirection UMETA(DisplayName = "EnemyDirection"),
	IsAlone UMETA(DisplayName = "IsAlone"),
	BreakLeftLeg UMETA(DisplayName = "BreakLeftLeg"),
	BreakRightLeg UMETA(DisplayName = "BreakRightLeg"),
	CanMovement UMETA(DisplayName = "CanMovement"),
	EnemyDensity UMETA(DisplayName = "EnemyDensity"),
	EnemyAverageDistance UMETA(DisplayName = "EnemyAverageDistance"),
	IsFarFromTarget UMETA(DisplayName = "IsFarFromTarget"),
	PoisonAmount UMETA(DisplayName = "PoisonAmount"),
	ElectricityAmount UMETA(DisplayName = "ElectricityAmount"),
	NearByCave UMETA(DisplayName = "NearByCave"),
	NearByDesert UMETA(DisplayName = "NearByDesert"),
	NearByRuin UMETA(DisplayName = "NearByRuin"),
	NearByBestCombatSpot UMETA(DisplayName = "NearByBestCombatSpot"),
	NeedCombatAreaChange UMETA(DisplayName = "NeedCombatAreaChange"),
	NeedCombatReposition UMETA(DisplayName = "NeedCombatReposition"),
	SurroundedRisk UMETA(DisplayName = "SurroundedRisk"),
	PositionalDisadvantage UMETA(DisplayName = "PositionalDisadvantage"),

	// Array - Target
	TargetDistanceNearly UMETA(DisplayName = "TargetDistanceNearly"),
	TargetHp UMETA(DisplayName = "TargetHp"),
	TargetPriority UMETA(DisplayName = "TargetPriority"),
	TargetWeakness UMETA(DisplayName = "TargetWeakness"),
	TargetPoisonedState UMETA(DisplayName = "TargetPoisonedState"),
	
};

UENUM(BlueprintType)
enum class EAxisType : uint8
{
	None,
	Single,
	Target,
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

	TArray<float> GetTargetDistanceNearly();
	TArray<float> GetTargetHp();
	TArray<float> GetTargetPriority();
	TArray<float> GetTargetWeakness();
	TArray<float> GetTargetPoisonedState();
	TArray<float> GetTargetAggroValue();

	// Axis
	float GetMyHp();
	float GetTarget();
	float GetMyCombatPotential();
	float GetIsTargetInSight();
	float GetNearbyEnemyCount();
	float GetThreatScore();
	float GetCombatDuration();
	float GetNearbyWaterAmount();
	float GetEnemyDirection();
	float GetIsAlone();
	float GetBreakRightLeg();
	float GetBreakLeftLeg();
	float GetCanMovement();
	float GetEnemyDensity();
	float GetEnemyAverageDistance();
	float GetIsFarFromTarget();

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
	const float MaxDamageForPriority = 100.0f; 

	float MyHp;
	
	bool bIsInCombat = false;
	float CombatStartTime = -1;

	float BestCombatTime;
	float ThreatCharacterNum;

	float EnemyDensity;
	float EnemyAverageDistance;

	float SelectedTargetDistance;
	
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

