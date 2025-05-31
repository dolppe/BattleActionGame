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
	Corner,
	HighArea,
	LowArea,
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

UCLASS()
class UBattleNavArea_Cave : public UBattleNavArea
{
	GENERATED_BODY()
public:
	UBattleNavArea_Cave(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
};


UCLASS()
class UBattleNavArea_Electricity : public UBattleNavArea
{
	GENERATED_BODY()
public:
	UBattleNavArea_Electricity(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
};



UCLASS()
class UBattleNavArea_Forest : public UBattleNavArea
{
	GENERATED_BODY()
public:
	UBattleNavArea_Forest(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};



UCLASS()
class UBattleNavArea_Plain : public UBattleNavArea
{
	GENERATED_BODY()
public:
	UBattleNavArea_Plain(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};



UCLASS()
class UBattleNavArea_RestingArea : public UBattleNavArea
{
	GENERATED_BODY()
public:
	UBattleNavArea_RestingArea(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};

UCLASS()
class UBattleNavArea_Water : public UBattleNavArea
{
	GENERATED_BODY()
public:
	UBattleNavArea_Water(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};

UCLASS()
class UBattleNavArea_Corner : public UBattleNavArea
{
	GENERATED_BODY()
public:
	UBattleNavArea_Corner(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};

UCLASS()
class UBattleNavArea_HighArea : public UBattleNavArea
{
	GENERATED_BODY()
public:
	UBattleNavArea_HighArea(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};

UCLASS()
class UBattleNavArea_LowArea : public UBattleNavArea
{
	GENERATED_BODY()
public:
	UBattleNavArea_LowArea(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};

