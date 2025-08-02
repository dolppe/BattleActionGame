#include "GridRiskMapComponent.h"

#include "WorldRiskGridMap.h"
#include "BattleActionGame/Character/BattleCharacter.h"
#include "BattleActionGame/Physics/BattleCollisionChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GridRiskMapComponent)

UGridRiskMapComponent::UGridRiskMapComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UGridRiskMapComponent::BeginPlay()
{
	Super::BeginPlay();

	RiskValues.AddZeroed(NumCols*NumRows);

	FVector OwnerLocation = GetOwner()->GetActorLocation();
	OwnerLocation.X = OwnerLocation.X - (NumCols/2 * CellSize);
	OwnerLocation.Y = OwnerLocation.Y - (NumRows/2 * CellSize);
	GridOrigin = OwnerLocation;
	
	UpdateRisk();

	GetWorld()->GetTimerManager().SetTimer(UpdateRiskTimerHandle, this, &ThisClass::UpdateRisk, 1.0f, true);


}

void UGridRiskMapComponent::UpdateRisk()
{
	
	UpdateCount++;

	if (UpdateCount == 100'000'000)
	{
		RiskValues.Empty();
		RiskValues.AddZeroed(NumCols*NumRows);

		UpdateCount = 1;
	}
	
	UpdateMovement();
	
	TArray<FOverlapResult> OutOverlaps;

	FVector Center = GridOrigin;
	Center.X = Center.X + (NumCols/2 * CellSize);
	Center.Y = Center.Y + (NumRows/2 * CellSize);

	FVector BoxExtender(NumCols/2*CellSize, NumRows/2*CellSize, 500.f);
	
	GetWorld()->OverlapMultiByChannel(OutOverlaps, Center, FQuat::Identity, Battle_TraceChannel_AttackToCharacter, FCollisionShape::MakeBox(BoxExtender));

	//DrawDebugBox(GetWorld(), Center, BoxExtender, FColor::Green, false, 10.0f, 0, 2.0f);
	
	for (FOverlapResult OverlapResult : OutOverlaps)
	{
		AActor* OverlapActor = OverlapResult.GetActor();

		if (ABattleCharacter* Character = Cast<ABattleCharacter>( OverlapActor))
		{
			int CharacterIdx = GetGridIdx(Character->GetActorLocation());

			if (CharacterIdx == -1)
			{
				continue;
			}

			SetRiskAtTargetIdx(CharacterIdx);
		}
	}
}

void UGridRiskMapComponent::UpdateMovement()
{
	FVector NewLocation = GetOwner()->GetActorLocation();
	NewLocation.X = NewLocation.X - (NumCols/2 * CellSize);
	NewLocation.Y = NewLocation.Y - (NumRows/2 * CellSize);

	GridOrigin = NewLocation;
}

void UGridRiskMapComponent::SetRiskAtTargetIdx(const int TargetIdx)
{
	TTuple<float, int>& GridItem = RiskValues[TargetIdx];

	

	GridItem.Key = 1.0f;
	GridItem.Value = TargetIdx;

	int TargetCol = TargetIdx % NumCols;
	int TargetRow = TargetIdx / NumCols;


	FVector LineStart = GridOrigin;
	LineStart.X += TargetCol*CellSize + CellSize*0.5f;
	LineStart.Y += TargetRow*CellSize + CellSize*0.5f;
	LineStart.Z = -5000.f;

	FVector LineEnd = LineStart;
	LineEnd.Z = 5000.f;

	//DrawDebugLine(GetWorld(), LineStart, LineEnd, FColor::Red, false, 10.0f, 0,10.0f);

	for (const FGridRiskOffset& RiskOffset : RiskOffsetTable)
	{
		int CurCol = RiskOffset.dX + TargetCol;
		int CurRow = RiskOffset.dY + TargetRow;

		int CurIdx = CurCol + (CurRow*NumCols);

		if (CurIdx < 0 || CurIdx >=(NumCols*NumRows))
		{
			continue;
		}
		FColor Color = FColor::Black;
		if (RiskValues[CurIdx].Key < RiskOffset.RiskValue || RiskValues[CurIdx].Value != UpdateCount)
		{
			RiskValues[CurIdx].Key = RiskOffset.RiskValue;
			RiskValues[CurIdx].Value = UpdateCount;
			if (FMath::IsNearlyEqual(RiskOffset.RiskValue, 1.0f))
			{
				Color = FColor::Red;
			}
			if (FMath::IsNearlyEqual(RiskOffset.RiskValue, 0.7f))
			{
				Color = FColor::Orange;
			}
			if (FMath::IsNearlyEqual(RiskOffset.RiskValue, 0.5f))
			{
				Color = FColor::Yellow;
			}
			if (FMath::IsNearlyEqual(RiskOffset.RiskValue, 0.3f))
			{
				Color = FColor::Black;
			}
			LineStart = GridOrigin;
			LineStart.X += CurCol*CellSize + CellSize*0.5f;
			LineStart.Y += CurRow*CellSize + CellSize*0.5f;
			LineStart.Z = -5000.f;

			LineEnd = LineStart;
			LineEnd.Z = 5000.f;
			
			//DrawDebugLine(GetWorld(), LineStart, LineEnd, Color, false, 10.0f, 0,10.0f);
			
		}
	}
	
}

float UGridRiskMapComponent::GetRiskValue(const FVector& Location)
{
	int Idx = GetGridIdx(Location);

	if (Idx == -1)
	{
		return -1;
	}

	return RiskValues[Idx].Key;
}

int UGridRiskMapComponent::GetGridIdx(const FVector& Location)
{
	if (!IsLocationInGrid(Location))
	{
		return -1;
	}

	float FirstGridX = GridOrigin.X;
	float FirstGridY = GridOrigin.Y;
	float DiffX = Location.X - FirstGridX;
	float DiffY = Location.Y - FirstGridY;
	
	int DiffColCount = DiffX / CellSize;
	int DiffRowCount = DiffY / CellSize;

	return DiffRowCount * NumCols + DiffColCount;
	
}

bool UGridRiskMapComponent::IsLocationInGrid(const FVector& Location)
{
	float FirstGridX = GridOrigin.X;
	float FirstGridY = GridOrigin.Y;

	float TargetX = Location.X;
	float TargetY = Location.Y;

	if (TargetX < FirstGridX || TargetY < FirstGridY)
		return false;
	if (TargetX >= FirstGridX + (CellSize * NumCols) || TargetY >= FirstGridY + (CellSize * NumRows))
		return false;
	
	return true;
	
}




