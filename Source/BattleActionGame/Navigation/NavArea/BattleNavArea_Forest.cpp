#include "BattleNavArea_Forest.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleNavArea_Forest)

UBattleNavArea_Forest::UBattleNavArea_Forest(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DefaultCost = 1.0f;
	DrawColor = FColor::Green;

	AreaFlags = AREA_Forest;
	TerrainType = ETerrainType::Forest;
}
