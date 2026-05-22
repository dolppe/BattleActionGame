#pragma once

#include "BattleActionGame/Camera/BattleCameraMode.h"
#include "GameFramework/Pawn.h"

#include "BattleActionGame/Character/BattlePawnData.h"
#include "BattleObserverPawn.generated.h"

class UBattleCameraComponent;
class UBattleInputConfig;

UCLASS()
class ABattleObserverPawn : public APawn
{
	GENERATED_BODY()
public:
	
	ABattleObserverPawn(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	void SetDefaultData(const UBattlePawnData& PawnData);
	
	virtual void PossessedBy(AController* NewController) override;
	
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

protected:
	
	UFUNCTION(Server, Reliable)
	void ServerReturnToCharacter();
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	TSubclassOf<UBattleCameraMode> DetermineCameraMode() const;
	void InitializePlayerInput(UInputComponent* PlayerInputComponent);
	
	void Input_LookMouse(const FInputActionValue& InputActionValue);
	void Input_ReturnToCharacter(const FInputActionValue& InputActionValue);
	
	UFUNCTION()
	void OnRep_DefaultCameraMode();
	UFUNCTION()
	void OnRep_InputData();
	UFUNCTION()
	void OnRep_InputConfigs();
private:
	
	UPROPERTY(ReplicatedUsing=OnRep_DefaultCameraMode)
	TSubclassOf<UBattleCameraMode> DefaultCameraMode;
	UPROPERTY(ReplicatedUsing=OnRep_InputData)
	TObjectPtr<UBattleInputConfig> InputData;
	UPROPERTY(ReplicatedUsing=OnRep_InputConfigs)
	TArray<FBattleMappableConfigPair> InputConfigs;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Battle|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBattleCameraComponent> CameraComponent;
	
};

