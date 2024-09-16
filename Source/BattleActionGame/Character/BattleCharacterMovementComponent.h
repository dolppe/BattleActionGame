#pragma once


#include "GameFramework/CharacterMovementComponent.h"
#include "BattleCharacterMovementComponent.generated.h"

UCLASS()
class BATTLEACTIONGAME_API UBattleCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
public:

	UBattleCharacterMovementComponent(const FObjectInitializer& ObjectInitializer);

	virtual void PerformMovement(float DeltaTime) override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	
};



