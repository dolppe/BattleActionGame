#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "WorldRiskGridMap.generated.h"

class UWorldRiskGridMapSubSystem;


USTRUCT(BlueprintType)
struct FRiskCellOffset
{
	GENERATED_BODY()
	
	FRiskCellOffset()
	{
		
	}
	
	FRiskCellOffset(int IndX, int IndY, float InRiskValue)
		: dX(IndX), dY(IndY), RiskWeight(InRiskValue)
	{
		
	}

	UPROPERTY(EditAnywhere)
	int dX;
	UPROPERTY(EditAnywhere)
	int dY;
	UPROPERTY(EditAnywhere)
	float RiskWeight;
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
	
	UPROPERTY(EditDefaultsOnly)
	float CellRiskValue = 2.0f;
};


struct FGridChunk
{

	FGridChunk(UWorldRiskGridMapSubSystem* InGridMapSubSystem, const FVector& InCenterLocation)
		: GridMapSubSystem(InGridMapSubSystem), CenterLocation(InCenterLocation)
	{
		
	}
	
	FGridChunk()
	{
		
	}
	
	FIntPoint GetCellIdx(const FVector& Location) const;
	FVector GetCellLocation(const FIntPoint& CellIdx) const;
	float GetRiskValue(const FVector& Location) const;
	float GetRiskValue(FIntPoint CellIdx) const;
	
	void UpdateChunk(TArray<FIntPoint>& NearlyChunks);
	void PrepareChunkUpdate(float UpdateTime);
	void SetRiskValue(const FVector& Location, TArray<FIntPoint>& NearlyChunks);
	void SetRiskValueOneCell(const FVector& Location, float InRiskValue);
	
	

	double CurUpdateTime = 0.0f;
	
	TMap<FIntPoint, float> RiskCellMap;
	
	UWorldRiskGridMapSubSystem* GridMapSubSystem;
	
	FVector CenterLocation;
	
};



UCLASS()
class UWorldRiskGridMapSubSystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	
	friend struct FGridChunk;

	void Init(const FGridMapInitData& InInitData);

	bool IsInit() const
	{
		return bIsInit;
	}
	
	float GetRiskValue(const FVector& Location);
	
	UFUNCTION(BlueprintCallable)
	void UpdateNearlyChunk(const FVector& Location);
	void SetRiskValue(const FVector& Location, float InRiskValue);
	
	FGridMapInitData InitData;
	
	bool GetChunk(const FVector& Location, FGridChunk& OutChunk);
	FIntPoint GetChunkIdx(const FVector& Location);
	

protected:

	bool IsLocationInGrid(const FVector& Location);
	TArray<FIntPoint> GetNearlyChunkIdx(const FVector& Location);
	
	
	
	
	bool bIsInit = false;
	
	TMap<FIntPoint, FGridChunk> RiskGridMap;

private:

	const int dx[8] = {1, 1, 1, 0, 0, -1, -1, -1};
	const int dy[8] = {1, 0, -1, 1, -1, 1, 0, -1};
	
};
