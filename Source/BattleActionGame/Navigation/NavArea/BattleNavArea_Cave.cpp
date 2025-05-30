#include "BattleNavArea_Cave.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleNavArea_Cave)

UBattleNavArea_Cave::UBattleNavArea_Cave(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DefaultCost = 1.0f;
	DrawColor = FColor::Purple;

	AreaFlags = AREA_Cave;
	TerrainType = ETerrainType::Cave;
	
}
