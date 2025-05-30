#include "BattleNavArea_Electricity.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleNavArea_Electricity)

UBattleNavArea_Electricity::UBattleNavArea_Electricity(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DefaultCost = 1.0f;
	DrawColor = FColor::Yellow;

	AreaFlags = AREA_Electricity;
	TerrainType = ETerrainType::Electricity;
	
}
