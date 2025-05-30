#pragma once

#include "NavAreas/NavArea.h"
#include "BattleNavArea.generated.h"

UENUM()
enum class ETerrainType : uint8
{
	None,
	Plain,
	Forest,
	Water,
	Cave,
	Electricity,
	RestingArea,
};

/*
 *      AreaFlag
 *
 *      bit 0 => 갈 수 없는 지형
 *      bit 1 => Plain
 *      bit 2 => Forest
 *      bit 3 => Water
 *      bit 4 => Cave
 *      bit 5 => Electricity
 *      bit 6 => RestingArea
 *      bit 7 => 고지대
 *      bit 8 => 저지대
 *      bit 9 => 구석 지형
 *      
 *      
 *      
 *      
 */

constexpr uint16 AREA_Unwalkable = 1 << 0;
constexpr uint16 AREA_Plain = 1 << 1;
constexpr uint16 AREA_Forest = 1 << 2;
constexpr uint16 AREA_Water = 1 << 3;
constexpr uint16 AREA_Cave = 1 << 4;
constexpr uint16 AREA_Electricity = 1 << 5;
constexpr uint16 AREA_RestingArea = 1 << 6;
constexpr uint16 AREA_HighArea = 1 << 7;
constexpr uint16 AREA_LowArea = 1 << 8;
constexpr uint16 AREA_Corner = 1 << 9;


UCLASS()
class UBattleNavArea : public UNavArea
{
	GENERATED_BODY()

public:

	UBattleNavArea(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	ETerrainType GetTerrainType()
	{
		return TerrainType;
	}



protected:

	ETerrainType TerrainType;
	
	
};


