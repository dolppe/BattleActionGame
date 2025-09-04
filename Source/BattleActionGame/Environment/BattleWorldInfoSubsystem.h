#pragma once


#include "BattleUtilitySpot.h"
#include "Subsystems/WorldSubsystem.h"
#include "BattleWorldInfoSubsystem.generated.h"

class ABattleUtilitySpot;

UCLASS()
class UBattleWorldInfoSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:

	virtual void PostInitialize() override;

	UFUNCTION(BlueprintCallable)
	ABattleUtilitySpot* GetUtilitySpot(EUtilitySpotType UtilitySpotType)
	{
		return UtilitySpotMap[UtilitySpotType];
	}


private:

	TMap<EUtilitySpotType, ABattleUtilitySpot*> UtilitySpotMap;

	
	
	
};
