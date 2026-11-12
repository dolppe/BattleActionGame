#include "WorldRiskGridMap.h"

#include "BattleActionGame/BattleLogChannels.h"
#include "BattleActionGame/Character/BattleCharacter.h"
#include "BattleActionGame/Physics/BattleCollisionChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(WorldRiskGridMap)

PRAGMA_DISABLE_OPTIMIZATION

FIntPoint FGridChunk::GetCellIdx(const FVector& Location) const
{
	float DiffX = Location.X - (CenterLocation.X - GridMapSubSystem->InitData.ChunkSize/2); 
	float DiffY = Location.Y - (CenterLocation.Y - GridMapSubSystem->InitData.ChunkSize/2);

	int DiffColCount = DiffX / GridMapSubSystem->InitData.CellSize;
	int DiffRowCount = DiffY / GridMapSubSystem->InitData.CellSize;

	return {DiffColCount, DiffRowCount};
	
}

FVector FGridChunk::GetCellLocation(const FIntPoint& CellIdx) const
{
	float ChunkStartX = CenterLocation.X - (GridMapSubSystem->InitData.ChunkSize/2);
	float ChunkStartY = CenterLocation.Y - (GridMapSubSystem->InitData.ChunkSize/2);
	
	float CellLocationX = ChunkStartX + GridMapSubSystem->InitData.CellSize*CellIdx.X;
	float CellLocationY = ChunkStartY + GridMapSubSystem->InitData.CellSize*CellIdx.Y;
	
	return FVector(CellLocationX, CellLocationY, 0.0f);
}

float FGridChunk::GetRiskValue(const FVector& Location) const
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
		return RiskCellMap[CellIdx];
	}
	else
	{
		//DrawDebugLine(GridMapSubSystem->GetWorld(), LineStart, LineEnd, FColor::Green, false, 10.0, 0, 5.0f);
		return 0.0f;
	}
}

float FGridChunk::GetRiskValue(FIntPoint CellIdx) const
{
	if (RiskCellMap.Contains(CellIdx))
	{
		return RiskCellMap[CellIdx];
	}
	
	return 0.0f;
}

void FGridChunk::UpdateChunk(TArray<FIntPoint>& NearlyChunks)
{	
	// 대충 Character랑 Enemy 찾는 행동 및 Cell 위험도 업데이트
	TArray<FOverlapResult> OutOverlaps;
	FVector BoxExtender(GridMapSubSystem->InitData.ChunkSize/2, GridMapSubSystem->InitData.ChunkSize/2, 3000.f);

	DrawDebugPoint(GridMapSubSystem->GetWorld(), CenterLocation, 5.0f, FColor::Blue, false, 20.f);
	
	GridMapSubSystem->GetWorld()->OverlapMultiByChannel(OutOverlaps, CenterLocation, FQuat::Identity, Battle_TraceChannel_AttackToCharacter, FCollisionShape::MakeBox(BoxExtender));

	for (const FOverlapResult& OverlapResult : OutOverlaps)
	{
		AActor* OverlapActor = OverlapResult.GetActor();
		
		if (ABattleCharacter* Character = Cast<ABattleCharacter>( OverlapActor))
		{
			SetRiskValue(Character->GetActorLocation(), NearlyChunks);
		}
	}
	
	
}

void FGridChunk::PrepareChunkUpdate(float UpdateTime)
{
	CurUpdateTime = UpdateTime;
	
	for (TTuple<FIntPoint, float>& RiskCellMapItem : RiskCellMap)
	{
		RiskCellMapItem.Value = 0.0f;
	}
}


void FGridChunk::SetRiskValue(const FVector& Location, TArray<FIntPoint>& NearlyChunks)
{
	FIntPoint CellIdx = GetCellIdx(Location);
	float RiskValue = GridMapSubSystem->InitData.CellRiskValue;

	if (RiskCellMap.Contains(CellIdx))
	{
		RiskCellMap[CellIdx] = RiskValue;
	}
	else
	{
		RiskCellMap.Add({CellIdx, RiskValue});
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
	
	//UE_LOG(LogBattle, Log, TEXT("SetRiskValue: %s"), *CenterLocation.ToString());

	for (const FRiskCellOffset& Offset : GridMapSubSystem->InitData.RiskOffsetTable)
	{
		FIntPoint CurIdx = CellIdx;
		CurIdx.X += Offset.dX;
		CurIdx.Y += Offset.dY;

		if (CurIdx.X < 0 || CurIdx.Y <0 || CurIdx.X >= MaxCellIdx || CurIdx.Y >= MaxCellIdx)
		{
			FVector OffsetLocation = Location;
			OffsetLocation.X += (Offset.dX*CellSize);
			OffsetLocation.Y += (Offset.dY*CellSize);
			
			FIntPoint TargetChunkIdx = GridMapSubSystem->GetChunkIdx(OffsetLocation);
			if (TargetChunkIdx == FIntPoint::NoneValue)
			{
				continue;
			}
			
			for (FIntPoint& NearlyChunkIdx : NearlyChunks)
			{
				if (TargetChunkIdx == NearlyChunkIdx)
				{
					GridMapSubSystem->SetRiskValue(OffsetLocation, Offset.RiskWeight * RiskValue);
				}
			}
			
			continue;
		}

		if (RiskCellMap.Contains(CurIdx))
		{
			if (RiskCellMap[CurIdx] < Offset.RiskWeight * RiskValue)
			{
				RiskCellMap[CurIdx] = Offset.RiskWeight * RiskValue;
				
				// if (FMath::IsNearlyEqual(Offset.RiskValue, 1.0f))
				// {
				// 	Color = FColor::Red;
				// }
				// if (FMath::IsNearlyEqual(Offset.RiskValue, 0.8f))
				// {
				// 	Color = FColor::Orange;
				// }
				// if (FMath::IsNearlyEqual(Offset.RiskValue, 0.5f))
				// {
				// 	Color = FColor::Yellow;
				// }
				// if (FMath::IsNearlyEqual(Offset.RiskValue, 0.3f))
				// {
				// 	Color = FColor::Black;
				// }
				//
				// LineStart = CenterLocation;
				// LineStart.X += CurIdx.X * CellSize - (GridMapSubSystem->InitData.ChunkSize/2);
				// LineStart.Y += CurIdx.Y * CellSize - (GridMapSubSystem->InitData.ChunkSize/2);
				// LineStart.Z = -5000.f;
				//
				// LineEnd = LineStart;
				// LineEnd.Z = 5000.f;
				//
				// DrawDebugBox(GridMapSubSystem->GetWorld(), GetCellLocation(CurIdx), FVector(GridMapSubSystem->InitData.CellSize/2),Color, false, 10.f, 0, 10.f);
			}
		}
		else
		{
			RiskCellMap.Add({CurIdx, Offset.RiskWeight * RiskValue});
			
			// if (FMath::IsNearlyEqual(Offset.RiskValue, 1.0f))
			// {
			// 	Color = FColor::Red;
			// }
			// if (FMath::IsNearlyEqual(Offset.RiskValue, 0.7f))
			// {
			// 	Color = FColor::Orange;
			// }
			// if (FMath::IsNearlyEqual(Offset.RiskValue, 0.5f))
			// {
			// 	Color = FColor::Yellow;
			// }
			// if (FMath::IsNearlyEqual(Offset.RiskValue, 0.3f))
			// {
			// 	Color = FColor::Black;
			// }
			//
			// LineStart = CenterLocation;
			// LineStart.X += CurIdx.X * CellSize - (GridMapSubSystem->InitData.ChunkSize/2);
			// LineStart.Y += CurIdx.Y * CellSize - (GridMapSubSystem->InitData.ChunkSize/2);
			// LineStart.Z = -5000.f;
			//
			// LineEnd = LineStart;
			// LineEnd.Z = 5000.f;
			//
			// DrawDebugBox(GridMapSubSystem->GetWorld(), GetCellLocation(CurIdx), FVector(GridMapSubSystem->InitData.CellSize/2),Color, false, 10.f, 0, 10.f);
		}

		
	}
}

void FGridChunk::SetRiskValueOneCell(const FVector& Location, float InRiskValue)
{
	FIntPoint CellIdx = GetCellIdx(Location);
	
	if (RiskCellMap.Contains(CellIdx))
	{
		if (RiskCellMap[CellIdx] < InRiskValue)
		{
			RiskCellMap[CellIdx] = InRiskValue;	
		}
		
	}
	else
	{
		RiskCellMap.Add({CellIdx, InRiskValue});
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

bool UWorldRiskGridMapSubSystem::GetChunk(const FVector& Location, FGridChunk& OutChunk)
{
	FIntPoint ChunkIdx = GetChunkIdx(Location);
	
	if (ChunkIdx == FIntPoint::NoneValue)
	{
		return false;
	}
	
	if (RiskGridMap.Contains(ChunkIdx))
	{
		OutChunk = RiskGridMap[ChunkIdx];
		return true;
	}
	return false;
	
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

	// 정해진 맵 범위 밖에 있으면 위험도 없음
	if (ChunkIdx == FIntPoint::NoneValue)
	{
		return 0.0f;
	}

	const float CurrentTime = GetWorld()->GetTimeSeconds();
	
	// 청크가 없거나 갱신 유효 시간이 지났을 때만 주변 영역 갱신
	if (!RiskGridMap.Contains(ChunkIdx) 
		|| CurrentTime - RiskGridMap[ChunkIdx].CurUpdateTime > InitData.AllowUpdateTime)
	{
		UpdateNearlyChunk(Location);
	}
	
	// 해당 Cell의 위험도를 반환
	return RiskGridMap[ChunkIdx].GetRiskValue(Location);
}

void UWorldRiskGridMapSubSystem::UpdateNearlyChunk(const FVector& Location)
{
	// 현 위치를 포함하여 주변 청크에 대한 Index값을 가져옴
	TArray<FIntPoint> NearlyChunkIndexes = GetNearlyChunkIdx(Location);
	const float CurTime = GetWorld()->GetTimeSeconds();

	// 없는 청크는 생성하고, 청크 내부의 위험도 초기화 및 갱신 시간 기록 
	for (const FIntPoint& ChunkKey : NearlyChunkIndexes)
	{
		if (!RiskGridMap.Contains(ChunkKey))
		{
			FVector ChunkCenter = InitData.StartLocation;
			ChunkCenter.X += InitData.ChunkSize/2 + (InitData.ChunkSize * ChunkKey.X);
			ChunkCenter.Y += InitData.ChunkSize/2 + (InitData.ChunkSize * ChunkKey.Y);
			
			RiskGridMap.Add(ChunkKey, FGridChunk(this, ChunkCenter));
		}
		
		RiskGridMap[ChunkKey].PrepareChunkUpdate(CurTime);
	}
	
	// 전체 초기화 후, 위험도 기록
	for (const FIntPoint& ChunkKey : NearlyChunkIndexes)
	{
		RiskGridMap[ChunkKey].UpdateChunk(NearlyChunkIndexes);
	}
}



void UWorldRiskGridMapSubSystem::SetRiskValue(const FVector& Location, float InRiskValue)
{
	FIntPoint ChunkIdx = GetChunkIdx(Location);
	
	if (ChunkIdx == FIntPoint::NoneValue)
	{
		return;
	}
	
	RiskGridMap[ChunkIdx].SetRiskValueOneCell(Location, InRiskValue);	
}


PRAGMA_ENABLE_OPTIMIZATION
