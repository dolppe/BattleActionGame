#pragma once
#include "NavFilters/NavigationQueryFilter.h"
#include "NavMesh/RecastNavMesh.h"
#include "NavMesh/RecastQueryFilter.h"
#include "WorldRiskGridMap.h"


#include "FindPathFunction.generated.h"


UCLASS()
class UNavQueryFilter_CustomCost : public UNavigationQueryFilter

{
	GENERATED_BODY()

public:

	UNavQueryFilter_CustomCost(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	
};

class FRecastQueryFilter_CustomCost : public FRecastQueryFilter
{
public:

	FRecastQueryFilter_CustomCost(bool bIsVirtual = true);

	virtual void SetAreaCost(uint8 AreaType, float Cost) override;
	virtual dtReal getVirtualCost(const dtReal* pa, const dtReal* pb, const dtPolyRef prevRef, const dtMeshTile* prevTile, const dtPoly* prevPoly, const dtPolyRef curRef, const dtMeshTile* curTile, const dtPoly* curPoly, const dtPolyRef nextRef, const dtMeshTile* nextTile, const dtPoly* nextPoly) const override;
	virtual float GetHeuristicScale() const override;

	virtual INavigationQueryFilterInterface* CreateCopy() const override;
	
	bool GetIsVirtual()
	{
		return isVirtual;
	}

	float Getasdfasdfasdf();

	static UWorldRiskGridMapSubSystem* RiskGridMapSubSystem;
	
};

UCLASS()
class ARecastNavMesh_CustomCost : public ARecastNavMesh
{
	GENERATED_BODY()
public:

	FRecastQueryFilter_CustomCost DefaultNavFilter;
	
	virtual void BeginPlay() override;
};


