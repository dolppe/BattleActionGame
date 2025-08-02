#include "WorldRiskGridMap.h"

#include "BattleActionGame/Character/BattleCharacter.h"
#include "BattleActionGame/Physics/BattleCollisionChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(WorldRiskGridMap)

PRAGMA_DISABLE_OPTIMIZATION

FIntPoint FGridChunk::GetCellIdx(const FVector& Location)
{
	float DiffX = Location.X - (CenterLocation.X - GridMapSubSystem->InitData.ChunkSize/2); 
	float DiffY = Location.Y - (CenterLocation.Y - GridMapSubSystem->InitData.ChunkSize/2);

	int DiffColCount = DiffX / GridMapSubSystem->InitData.CellSize;
	int DiffRowCount = DiffY / GridMapSubSystem->InitData.CellSize;

	return {DiffColCount, DiffRowCount};
	
}

float FGridChunk::GetRiskValue(const FVector& Location)
{
	FIntPoint CellIdx = GetCellIdx(Location);

	//
	// FVector LineStart = Location;
	// LineStart.Z = -2000.f;
	//
	// FVector LineEnd = LineStart;
	// LineEnd.Z = 5000.f;

	if (RiskCellMap.Contains(CellIdx))
	{
		//DrawDebugLine(GridMapSubSystem->GetWorld(), LineStart, LineEnd, FColor::Red, false, 10.0, 0, 5.0f);
		return RiskCellMap[CellIdx].RiskValue;
	}
	else
	{
		//DrawDebugLine(GridMapSubSystem->GetWorld(), LineStart, LineEnd, FColor::Green, false, 10.0, 0, 5.0f);
		return 0.0f;
	}
}

void FGridChunk::UpdateChunk()
{	
	// 대충 Character랑 Enemy 찾는 행동 및 Cell 위험도 업데이트

	UpdateTime = GridMapSubSystem->GetWorld()->GetTimeSeconds();

	TArray<FOverlapResult> OutOverlaps;
	FVector BoxExtender(GridMapSubSystem->InitData.ChunkSize/2, GridMapSubSystem->InitData.ChunkSize/2, 3000.f);

	//DrawDebugPoint(GridMapSubSystem->GetWorld(), CenterLocation, 5.0f, FColor::Blue, false, 20.f);
	
	GridMapSubSystem->GetWorld()->OverlapMultiByChannel(OutOverlaps, CenterLocation, FQuat::Identity, Battle_TraceChannel_AttackToCharacter, FCollisionShape::MakeBox(BoxExtender));

	for (const FOverlapResult& OverlapResult : OutOverlaps)
	{
		AActor* OverlapActor = OverlapResult.GetActor();
		
		if (ABattleCharacter* Character = Cast<ABattleCharacter>( OverlapActor))
		{
			SetRiskValue(Character->GetActorLocation());
		}
	}
	
	
}

void FGridChunk::SetRiskValue(const FVector& Location)
{
	FIntPoint CellIdx = GetCellIdx(Location);

	if (RiskCellMap.Contains(CellIdx))
	{
		RiskCellMap[CellIdx].RiskValue = 1.0f;
	}
	else
	{
		RiskCellMap.Add({CellIdx, FRiskCell(1.0f, UpdateTime)});
	}

	float CellSize = GridMapSubSystem->InitData.CellSize;

	FVector LineStart = CenterLocation;
	LineStart.X += CellIdx.X*CellSize - (GridMapSubSystem->InitData.ChunkSize/2);
	LineStart.Y += CellIdx.Y*CellSize - (GridMapSubSystem->InitData.ChunkSize/2);
	LineStart.Z = -5000.f;

	FVector LineEnd = LineStart;
	LineEnd.Z = 5000.f;

	FColor Color = FColor::Red;

	//DrawDebugLine(GridMapSubSystem->GetWorld(), LineStart, LineEnd, FColor::Red, false, 10.0f, 0,10.0f);

	int MaxCellIdx = GridMapSubSystem->InitData.ChunkSize / GridMapSubSystem->InitData.CellSize;

	for (const FRiskCellOffset& Offset : GridMapSubSystem->InitData.RiskOffsetTable)
	{
		FIntPoint CurIdx = CellIdx;
		CurIdx.X += Offset.dX;
		CurIdx.Y += Offset.dY;

		if (CurIdx.X < 0 || CurIdx.Y <0 || CurIdx.X >= MaxCellIdx || CurIdx.Y >= MaxCellIdx)
		{
			continue;
		}

		if (RiskCellMap.Contains(CurIdx))
		{
			if (RiskCellMap[CurIdx].RiskValue < Offset.RiskValue || RiskCellMap[CurIdx].UpdateTime < UpdateTime)
			{
				RiskCellMap[CurIdx].RiskValue = Offset.RiskValue;
				RiskCellMap[CurIdx].UpdateTime = UpdateTime;
			}
		}
		else
		{
			RiskCellMap.Add({CurIdx, FRiskCell(Offset.RiskValue, UpdateTime)});
		}

		if (FMath::IsNearlyEqual(Offset.RiskValue, 1.0f))
		{
			Color = FColor::Red;
		}
		if (FMath::IsNearlyEqual(Offset.RiskValue, 0.7f))
		{
			Color = FColor::Orange;
		}
		if (FMath::IsNearlyEqual(Offset.RiskValue, 0.5f))
		{
			Color = FColor::Yellow;
		}
		if (FMath::IsNearlyEqual(Offset.RiskValue, 0.3f))
		{
			Color = FColor::Black;
		}

		LineStart = CenterLocation;
		LineStart.X += CurIdx.X * CellSize - (GridMapSubSystem->InitData.ChunkSize/2);
		LineStart.Y += CurIdx.Y * CellSize - (GridMapSubSystem->InitData.ChunkSize/2);
		LineStart.Z = -5000.f;

		LineEnd = LineStart;
		LineEnd.Z = 5000.f;

		//DrawDebugLine(GridMapSubSystem->GetWorld(), LineStart, LineEnd, Color, false, 10.0f, 0,10.0f);
	}
}

void UWorldRiskGridMapSubSystem::Init(const FGridMapInitData& InInitData)
{
	InitData = InInitData;


	
	bIsInit = true;
}

bool UWorldRiskGridMapSubSystem::IsLocationInGrid(const FVector& Location)
{
	if (Location.X < InitData.StartLocation.X || Location.Y < InitData.StartLocation.Y)
	{
		return false;
	}
	if (Location.X >= InitData.StartLocation.X + (InitData.ChunkSize * InitData.NumChunkCols) || Location.Y >= InitData.StartLocation.Y + (InitData.ChunkSize * InitData.NumChunkRows))
	{
		return false;
	}

	return true;
}

TArray<FIntPoint> UWorldRiskGridMapSubSystem::GetNearlyChunkIdx(const FVector& Location)
{
	TArray<FIntPoint> Result;

	FIntPoint ChunkIdx = GetChunkIdx(Location);

	if (ChunkIdx == FIntPoint::NoneValue)
	{
		return Result;
	}

	Result.Add(ChunkIdx);

	for (int dir = 0; dir <8;dir++)
	{
		FIntPoint Cur = {ChunkIdx.X+dx[dir], ChunkIdx.Y+dy[dir]};

		if (Cur.X < 0 || Cur.Y <0 || Cur.X >=InitData.NumChunkCols || Cur.Y >= InitData.NumChunkRows)
		{
			continue;
		}

		Result.Add(Cur);
	}
	
	return Result;
}

FIntPoint UWorldRiskGridMapSubSystem::GetChunkIdx(const FVector& Location)
{
	if (!IsLocationInGrid(Location))
	{
		return FIntPoint::NoneValue;
	}

	float DiffX = Location.X - InitData.StartLocation.X; 
	float DiffY = Location.Y - InitData.StartLocation.Y;

	int DiffColCount = DiffX / InitData.ChunkSize;
	int DiffRowCount = DiffY / InitData.ChunkSize;

	return {DiffColCount, DiffRowCount};
	
	
}

float UWorldRiskGridMapSubSystem::GetRiskValue(const FVector& Location)
{
	const FIntPoint ChunkIdx = GetChunkIdx(Location);

	if (ChunkIdx == FIntPoint::NoneValue)
	{
		return 0.0f;
	}

	if (!GridRiskMap.Contains(ChunkIdx))
	{
		FVector ChunkCenter = InitData.StartLocation;
		ChunkCenter.X += InitData.ChunkSize/2 + (InitData.ChunkSize * ChunkIdx.X);
		ChunkCenter.Y += InitData.ChunkSize/2 + (InitData.ChunkSize * ChunkIdx.Y);
		
		GridRiskMap.Add({ChunkIdx, FGridChunk(this, ChunkCenter)});
		GridRiskMap[ChunkIdx].UpdateChunk();
	}
	else if (GetWorld()->GetTimeSeconds() - GridRiskMap[ChunkIdx].UpdateTime > InitData.AllowUpdateTime)
	{
		GridRiskMap[ChunkIdx].UpdateChunk();
	}

	return GridRiskMap[ChunkIdx].GetRiskValue(Location);
}

void UWorldRiskGridMapSubSystem::UpdateRisk(const FVector& Location)
{
	TArray<FIntPoint> UpdateChunk = GetNearlyChunkIdx(Location);

	for (const FIntPoint& ChunkKey : UpdateChunk)
	{
		FVector ChunkCenter = InitData.StartLocation;
		ChunkCenter.X += InitData.ChunkSize/2 + (InitData.ChunkSize * ChunkKey.X);
		ChunkCenter.Y += InitData.ChunkSize/2 + (InitData.ChunkSize * ChunkKey.Y);
		
		if (!GridRiskMap.Contains(ChunkKey))
		{

			
			GridRiskMap.Add(ChunkKey, FGridChunk(this, ChunkCenter));

			
		}

		FVector BoxExtender(InitData.ChunkSize/2, InitData.ChunkSize/2, 2000);

		//DrawDebugBox(GetWorld(), ChunkCenter, BoxExtender, FColor::Green, false, 20.0f, 0, 5.0f);
		
		GridRiskMap[ChunkKey].UpdateChunk();
	}
	
}

PRAGMA_ENABLE_OPTIMIZATION
