#pragma once

#include "GridRiskMapComponent.generated.h"

USTRUCT(BlueprintType)
struct FGridRiskOffset
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	int dX;
	UPROPERTY(EditAnywhere)
	int dY;
	UPROPERTY(EditAnywhere)
	float RiskValue;
};


UCLASS(Blueprintable, Meta=(BlueprintSpawnableComponent))
class UGridRiskMapComponent : public UActorComponent
{
	GENERATED_BODY()
public:

	UGridRiskMapComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	void UpdateRisk();
	float GetRiskValue(const FVector& Location);
	

protected:
	
	int GetGridIdx(const FVector& Location);
	bool IsLocationInGrid(const FVector& Location);
	void UpdateMovement();

	void SetRiskAtTargetIdx(const int TargetIdx);
	
	/*
	 * 초기 설정
	 */

	UPROPERTY(EditDefaultsOnly)
	int NumCols = 10;
	
	UPROPERTY(EditDefaultsOnly)
	int NumRows = 10;
	
	UPROPERTY(EditDefaultsOnly)
	float CellSize = 1000.f;

	UPROPERTY(EditDefaultsOnly)
	TArray<FGridRiskOffset> RiskOffsetTable;

	/*
	 * World 좌표 관련
	 */
	
	// 현재 위치 기준 가장 왼쪽 아래의 Grid에 대한 Location
	FVector GridOrigin;

	/*
	 * 가상의 위험도 Grid
	 */
	
	// 실제 위험도가 들어갈 Map
	// [위험도, 업데이트 Count]
	TArray<TTuple<float, int>> RiskValues;
	int UpdateCount = 0;
	FTimerHandle UpdateRiskTimerHandle;
	
};
