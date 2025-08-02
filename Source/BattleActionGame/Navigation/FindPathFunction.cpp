#include "FindPathFunction.h"


#include "BattleActionGame/BattleLogChannels.h"
#include "BattleActionGame/GameModes/BattleWorldSettings.h"
#include "NavAreas/NavArea_Default.h"
#include "NavAreas/NavArea_LowHeight.h"
#include "NavAreas/NavArea_Null.h"
#include "NavMesh/PImplRecastNavMesh.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(FindPathFunction)

UWorldRiskGridMapSubSystem* FRecastQueryFilter_CustomCost::RiskGridMapSubSystem = nullptr;

UNavQueryFilter_CustomCost::UNavQueryFilter_CustomCost(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

FRecastQueryFilter_CustomCost::FRecastQueryFilter_CustomCost(bool bIsVirtual)
	: FRecastQueryFilter(bIsVirtual)
{
}

void FRecastQueryFilter_CustomCost::SetAreaCost(uint8 AreaType, float Cost)
{
	FRecastQueryFilter::SetAreaCost(AreaType, Cost);
}

dtReal FRecastQueryFilter_CustomCost::getVirtualCost(const dtReal* pa, const dtReal* pb, const dtPolyRef prevRef,
	const dtMeshTile* prevTile, const dtPoly* prevPoly, const dtPolyRef curRef, const dtMeshTile* curTile,
	const dtPoly* curPoly, const dtPolyRef nextRef, const dtMeshTile* nextTile, const dtPoly* nextPoly) const
{
	dtReal Distance = dtVdist(pa, pb);
	
	float RiskWeight = 2.0f;
	float RiskValue = RiskGridMapSubSystem->GetRiskValue(FVector(-pb[0], -pb[2], pb[1])) * RiskWeight * Distance;

	const dtReal areaChangeCost = nextPoly != 0 && nextPoly->getArea() != curPoly->getArea()
		? data.m_areaFixedCost[nextPoly->getArea()] : 0.f;

	return Distance * data.m_areaCost[curPoly->getArea()] + areaChangeCost + RiskValue;
}

PRAGMA_DISABLE_OPTIMIZATION

float FRecastQueryFilter_CustomCost::GetHeuristicScale() const
{
	return 100000000;
}

INavigationQueryFilterInterface* FRecastQueryFilter_CustomCost::CreateCopy() const
{
	return new FRecastQueryFilter_CustomCost(*this);
}

float FRecastQueryFilter_CustomCost::Getasdfasdfasdf()
{
	return 10000.f;
}


void ARecastNavMesh_CustomCost::BeginPlay()
{
	Super::BeginPlay();
	
	const INavigationQueryFilterInterface* QueryFilterInterface = dynamic_cast<const INavigationQueryFilterInterface*>(&DefaultNavFilter);
	DefaultQueryFilter->SetFilterImplementation(QueryFilterInterface);

	UWorldRiskGridMapSubSystem* GridMapSubSystem = GetWorld()->GetSubsystem<UWorldRiskGridMapSubSystem>();
	ABattleWorldSettings* WorldSettings = Cast<ABattleWorldSettings>(GetWorld()->GetWorldSettings());
	GridMapSubSystem->Init(WorldSettings->GetGridMapInitData());

	FRecastQueryFilter_CustomCost::RiskGridMapSubSystem = GridMapSubSystem;
	
	
}

PRAGMA_ENABLE_OPTIMIZATION
