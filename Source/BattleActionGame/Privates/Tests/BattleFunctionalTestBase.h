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
	
	void PlayerControllerReady();
	void GameplayReady();
	
	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnPlayerDataReady")
	void OnPlayerDataReady();
	
	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnGameplayReady")
	void OnGameplayReady();
	
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
	
	UFUNCTION(BlueprintCallable)
	void ClientInputPressRepeat(const FKey& Key, float Duration, float Period);
	
	UFUNCTION(BlueprintCallable)
	void ClientInputAxis(const FKey& Key, const FVector& Delta);
	
	UFUNCTION(BlueprintCallable)
	void ClientInputInjection(const UInputAction* InputAction);
	
	
protected:
	
	bool bPlayerDataReady = false;
	ABattleFunctionalTestPlayerController* PlayerController;
	
};
