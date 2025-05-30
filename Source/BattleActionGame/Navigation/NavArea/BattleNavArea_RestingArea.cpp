#include "BattleNavArea_RestingArea.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleNavArea_RestingArea)

UBattleNavArea_RestingArea::UBattleNavArea_RestingArea(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DefaultCost = 1.0f;
	DrawColor = FColor::Emerald;

	AreaFlags = AREA_RestingArea;
	TerrainType = ETerrainType::RestingArea;
}
