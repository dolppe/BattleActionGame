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
	
	const dtReal areaChangeCost = 
		nextPoly != 0 && nextPoly->getArea() != curPoly->getArea()
		? data.m_areaFixedCost[nextPoly->getArea()] : 0.f;
	
	// 기존 경로 비용
	const dtReal BaseCost = 
		Distance * data.m_areaCost[curPoly->getArea()] + areaChangeCost;
	
	if (RiskGridMapSubSystem == nullptr)
	{
		return BaseCost;
	}
	
	// Detour 좌표를 Unreal 좌표로 변환하여 위험도 조회
	const FVector SampleLocation(-pb[0], -pb[2], pb[1]);
	const float RiskValue = RiskGridMapSubSystem->GetRiskValue(SampleLocation);
	
	// 위험도 x 구간 거리를 추가 비용으로 반영
	const dtReal RiskCost = RiskValue * Distance;
	return BaseCost + RiskCost;
	
}

PRAGMA_DISABLE_OPTIMIZATION

float FRecastQueryFilter_CustomCost::GetHeuristicScale() const
{
	return 1.0f;
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
	
	
	// FRecastQueryFilter_CustomCost 타입의 DefaultNavFilter를 기본 쿼리 필터에 연결
	const INavigationQueryFilterInterface* QueryFilterInterface = 
		dynamic_cast<const INavigationQueryFilterInterface*>(&DefaultNavFilter);
	
	DefaultQueryFilter->SetFilterImplementation(QueryFilterInterface);

	
	UWorldRiskGridMapSubSystem* GridMapSubSystem = GetWorld()->GetSubsystem<UWorldRiskGridMapSubSystem>();
	ABattleWorldSettings* WorldSettings = Cast<ABattleWorldSettings>(GetWorld()->GetWorldSettings());
	GridMapSubSystem->Init(WorldSettings->GetGridMapInitData());

	FRecastQueryFilter_CustomCost::RiskGridMapSubSystem = GridMapSubSystem;
	
	
}

PRAGMA_ENABLE_OPTIMIZATION
