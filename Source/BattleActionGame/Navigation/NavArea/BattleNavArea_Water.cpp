#include "BattleNavArea_Water.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleNavArea_Water)

UBattleNavArea_Water::UBattleNavArea_Water(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DefaultCost = 1.0f;
	DrawColor = FColor::Blue;

	AreaFlags = AREA_Water;
	TerrainType = ETerrainType::Water;
}
