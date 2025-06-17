#include "BattleNavArea.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleNavArea)

UBattleNavArea::UBattleNavArea(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	TerrainType = ETerrainType::Plain;
}

UBattleNavArea_Cave::UBattleNavArea_Cave(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DefaultCost = 1.0f;
	DrawColor = FColor::Purple;

	AreaFlags = AREA_Cave;
	TerrainType = ETerrainType::Cave;
	
}

UBattleNavArea_Desert::UBattleNavArea_Desert(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DefaultCost = 1.0f;
	DrawColor = FColor::Yellow;

	AreaFlags = AREA_Desert;
	TerrainType = ETerrainType::Desert;
}

UBattleNavArea_Forest::UBattleNavArea_Forest(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DefaultCost = 1.0f;
	DrawColor = FColor::Green;

	AreaFlags = AREA_Forest;
	TerrainType = ETerrainType::Forest;
}

UBattleNavArea_Plain::UBattleNavArea_Plain(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DefaultCost = 1.0f;
	DrawColor = FColor::Silver;

	AreaFlags = AREA_Plain;
	TerrainType = ETerrainType::Plain;
}

UBattleNavArea_RestingArea::UBattleNavArea_RestingArea(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DefaultCost = 1.0f;
	DrawColor = FColor::Emerald;

	AreaFlags = AREA_RestingArea;
	TerrainType = ETerrainType::RestingArea;
}

UBattleNavArea_Ruin::UBattleNavArea_Ruin(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DefaultCost = 1.0f;
	DrawColor = FColor::Blue;

	AreaFlags = AREA_Ruin;
	TerrainType = ETerrainType::Ruin;
}

UBattleNavArea_Corner::UBattleNavArea_Corner(const FObjectInitializer& ObjectInitializer)
{
	DefaultCost = 1.0f;
	DrawColor = FColor::Black;

	AreaFlags = AREA_Corner;
	TerrainType = ETerrainType::Corner;
}

UBattleNavArea_HighArea::UBattleNavArea_HighArea(const FObjectInitializer& ObjectInitializer)
{
	DefaultCost = 1.0f;
    DrawColor = FColor::Turquoise;

    AreaFlags = AREA_HighArea;
    TerrainType = ETerrainType::HighArea;
}

UBattleNavArea_LowArea::UBattleNavArea_LowArea(const FObjectInitializer& ObjectInitializer)
{
	DefaultCost = 1.0f;
	DrawColor = FColor::Orange;

	AreaFlags = AREA_LowArea;
	TerrainType = ETerrainType::LowArea;
}
