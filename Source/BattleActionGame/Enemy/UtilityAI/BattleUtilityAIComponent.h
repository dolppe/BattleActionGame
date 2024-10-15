#pragma once

#include "Components/PawnComponent.h"
#include "BattleUtilityAIComponent.generated.h"

enum class EBattleConsiderType : uint8;
class UBattleUtilityAIData;
class UBattleUtilityAction;

USTRUCT()
struct FConsiderationFactors
{
	GENERATED_BODY()

	TMap<EBattleConsiderType, float> Factors;
	
};

UCLASS()
class UBattleUtilityAIComponent : public UPawnComponent
{
	GENERATED_BODY()
public:

	UBattleUtilityAIComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;

	void CollectConsiderFactors();
	
	void SelectBestAction();

	void StartTimer();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	
	TObjectPtr<const UBattleUtilityAIData> UtilityAIData;
	TArray<TObjectPtr<UBattleUtilityAction>> InstancedActions;

	FConsiderationFactors ConsiderList;

	UBattleUtilityAction* ActiveAction;
	bool bActionComplete;

	// 특정 거리에 Character가 있는지 체크 => 있으면 계산 주기를 0.5, 없으면 1.0 => 3.0 => 5.0...
	// 캐릭터 발견 시 즉각 반응하도록
	
	float UpdatePeriod = 0.5f;

	FTimerHandle TimerHandle;
	
	
};

