#pragma once

#include "BattleUtilitySpot.generated.h"

UENUM()
enum class EUtilitySpotType : uint8
{
	None,
	Resting,
	ChargeElectricity,
	ChargePoison,
	GameStart,
};


UCLASS()
class ABattleUtilitySpot : public AActor
{
	GENERATED_BODY()

public:
	
	ABattleUtilitySpot();

	UFUNCTION(BlueprintImplementableEvent)
	void ExecuteInteraction(ACharacter* TargetCharacter);

	UFUNCTION()
	EUtilitySpotType GetSpotType() const
	{
		return UtilitySpotType;
	}

protected:

	UPROPERTY(EditAnywhere)
	EUtilitySpotType UtilitySpotType;
		
};
