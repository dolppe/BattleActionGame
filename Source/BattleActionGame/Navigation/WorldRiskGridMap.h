#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "WorldRiskGridMap.generated.h"

class UWorldRiskGridMapSubSystem;


USTRUCT(BlueprintType)
struct FRiskCellOffset
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	int dX;
	UPROPERTY(EditAnywhere)
	int dY;
	UPROPERTY(EditAnywhere)
	float RiskValue;
};

USTRUCT()
struct FGridMapInitData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TArray<FRiskCellOffset> RiskOffsetTable;
	
	UPROPERTY(EditDefaultsOnly)
	FVector StartLocation;
	UPROPERTY(EditDefaultsOnly)
	float ChunkSize;

	UPROPERTY(EditDefaultsOnly)
	int NumChunkCols;
	UPROPERTY(EditDefaultsOnly)
	int NumChunkRows;

	UPROPERTY(EditDefaultsOnly)
	float CellSize;

	UPROPERTY(EditDefaultsOnly)
	float AllowUpdateTime = 5.0f;
};




struct FRiskCell
{
	FRiskCell(float InRiskValue, float InUpdateTime)
		: RiskValue(InRiskValue), UpdateTime(InUpdateTime)
	{
		
	}
	
	float RiskValue = 0.0f;
	float UpdateTime = 0.0f;
	
};

struct FGridChunk
{

	FGridChunk(UWorldRiskGridMapSubSystem* InGridMapSubSystem, const FVector& InCenterLocation)
		: GridMapSubSystem(InGridMapSubSystem), CenterLocation(InCenterLocation)
	{
		
	}
	
	FIntPoint GetCellIdx(const FVector& Location);
	float GetRiskValue(const FVector& Location);
	
	void UpdateChunk();
	void SetRiskValue(const FVector& Location);
	
	

	double UpdateTime = 0.0f;
	
	TMap<FIntPoint, FRiskCell> RiskCellMap;
	
	UWorldRiskGridMapSubSystem* GridMapSubSystem;
	
	FVector CenterLocation;
	
};



UCLASS()
class UWorldRiskGridMapSubSystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:

	void Init(const FGridMapInitData& InInitData);

	bool IsInit() const
	{
		return bIsInit;
	}
	
	float GetRiskValue(const FVector& Location);
	void UpdateRisk(const FVector& Location);

	FGridMapInitData InitData;

protected:

	bool IsLocationInGrid(const FVector& Location);
	TArray<FIntPoint> GetNearlyChunkIdx(const FVector& Location);
	FIntPoint GetChunkIdx(const FVector& Location);
	
	bool bIsInit = false;
	
	TMap<FIntPoint, FGridChunk> GridRiskMap;

private:

	const int dx[8] = {1, 1, 1, 0, 0, -1, -1, -1};
	const int dy[8] = {1, 0, -1, 1, -1, 1, 0, -1};
	
};
