#pragma once

#include "FunctionalTest.h"
#include "BattleActionGame/Player/BattleFunctionalTestPlayerController.h"
#include "BattleFunctionalTestBase.generated.h"

UCLASS()
class ABattleFunctionalTestBase : public AFunctionalTest
{
	GENERATED_BODY()
public:
	ABattleFunctionalTestBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual void Tick(float DeltaSeconds) override;
	
	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnPlayerDataReady")
	void OnPlayerDataReady();
	
	UFUNCTION(BlueprintCallable)
	ABattleFunctionalTestPlayerController* GetTestPlayerController()
	{
		return PlayerController;
	}
	
	bool IsPlayerDataReady() const
	{
		return bPlayerDataReady;
	}
	
	UFUNCTION(BlueprintCallable)
	void ClientInputPress(const FKey& Key);
	
	
	
	
protected:
	
	bool bPlayerDataReady = false;
	ABattleFunctionalTestPlayerController* PlayerController;
	
};
