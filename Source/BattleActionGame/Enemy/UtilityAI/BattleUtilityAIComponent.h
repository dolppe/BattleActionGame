#pragma once

#include "Components/PawnComponent.h"
#include "VisualLogger/VisualLoggerTypes.h"
#include "BattleUtilityAIComponent.generated.h"


class UBattleUtilityAIComponent;
class ABattleCharacterBase;
class UBattleUtilityAIData;
class UBattleUtilityAction;

UENUM(BlueprintType)
enum class EBattleConsiderType : uint8
{
	// Single
	MyHp UMETA(DisplayName = "MyHp"),
	HasTarget UMETA(DisplayName = "HasTarget"),
	MyCombatPotential UMETA(DisplayName = "MyCombatPotential"),

	// Array
	TargetDistanceNearly UMETA(DisplayName = "TargetDistanceNearly"),
	TargetHp UMETA(DisplayName = "TargetHp"),
	TargetPriority UMETA(DisplayName = "TargetPriority"),
	TargetWeakness UMETA(DisplayName = "TargetWeakness")
	
};

UENUM(BlueprintType)
enum class EAxisType : uint8
{
	None,
	Single,
	Target,
};

DECLARE_ENUM_TO_STRING(EBattleConsiderType);

UCLASS()
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

	// Axis
	float GetMyHp();
	float GetTarget();
	float GetMyCombatPotential();

	TFunction<float()> GetConsiderFunction(EBattleConsiderType ConsiderType);
	TFunction<TArray<float>()> GetArrayConsiderFunction(EBattleConsiderType ConsiderType);

	EAxisType GetAxisType(EBattleConsiderType ConsiderType);
	
	void InitConsiderFunction(const UBattleUtilityAIData* UtilityAIData);
	void ClearConsiderFactors();


	/*
	 * 상황 정보를 알아오는 함수
	 */

	void GetConsiderListData();
	void SearchNearActors();

	AActor* GetTargetPtr(EAxisType InAxisType, int Index) const;
	
public:

	TMap<EBattleConsiderType, EAxisType> ConsiderTypeMap;
	
	TMap<EBattleConsiderType, float> Factors;
	TMap<EBattleConsiderType, TFunction<float()>> Functions;

	TMap<EBattleConsiderType, TArray<float>> ArrayFactors;
	TMap<EBattleConsiderType, TFunction<TArray<float>()>> ArrayFunctions;

	TObjectPtr<ABattleCharacterBase> SelectedTarget;
	TArray<TObjectPtr<ABattleCharacterBase>> TargetActors;
	TArray<float> TargetDistances;
	TArray<float> TargetHps;

	float MyHp;
	
	TObjectPtr<ABattleCharacterBase> MyCharacter;
	TObjectPtr<UBattleUtilityAIComponent> UtilityAIComponent;
	
	
};


UCLASS(Blueprintable, Meta=(BlueprintSpawnableComponent))
class UBattleUtilityAIComponent : public UPawnComponent
{
	GENERATED_BODY()
public:

	UBattleUtilityAIComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;

	virtual void PostInitProperties() override;
	

	void CollectConsiderFactors();
	
	void SelectBestAction();

	void StartTimer();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	friend class UBattleUtilityAxis;
	friend class UBattleUtilityArrayAxis;
	friend class UBattleUtilityAction;
	friend class UConsiderationFactors;

	UPROPERTY()
	TObjectPtr<UConsiderationFactors> ConsiderList;
	
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



	
	
};

