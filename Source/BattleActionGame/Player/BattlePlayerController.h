#pragma once

#include "ModularPlayerController.h"
#include "GameFramework/PlayerController.h"
#include "BattlePlayerController.generated.h"

class ABattlePlayerState;
class UBattleAbilitySystemComponent;

UCLASS()
class ABattlePlayerController : public AModularPlayerController
{
	GENERATED_BODY()

public:
	ABattlePlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	
	ABattlePlayerState* GetBattlePlayerState() const;
	UBattleAbilitySystemComponent* GetBattleAbilitySystemComponent() const;
	
};

