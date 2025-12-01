#pragma once

#include "BattleCameraMode.h"
#include "BattleCameraMode_DesiredViewPoint.generated.h"

class UBattleHeroComponent;

UCLASS(Blueprintable)
class UBattleCameraMode_DesiredViewPoint : public UBattleCameraMode
{
	GENERATED_BODY()
public:
	UBattleCameraMode_DesiredViewPoint(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void UpdateView(float DeltaTime) override;

private:

	UBattleHeroComponent* CachedHeroComponent;
	
};
