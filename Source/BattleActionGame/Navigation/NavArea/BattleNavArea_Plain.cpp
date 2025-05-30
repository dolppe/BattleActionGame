#include "BattleNavArea_Plain.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleNavArea_Plain)

UBattleNavArea_Plain::UBattleNavArea_Plain(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DefaultCost = 1.0f;
	DrawColor = FColor::Silver;

	AreaFlags = AREA_Plain;
	TerrainType = ETerrainType::Plain;
	
	
	
}
