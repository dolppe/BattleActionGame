#pragma once

#include "GameplayAbilitySpecHandle.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "Components/PawnComponent.h"
#include "InputActionValue.h"
#include "BattleActionGame/Input/BattleInputConfig.h"
#include "BattleHeroComponent.generated.h"

class UBattleCameraSequencePlayer;
struct FBattleCameraModeInitData;
class UAbilitySystemComponent;
class UBattleCameraMode;
struct FGameplayAbilitySpecHandle;
struct FBattleMappableConfigPair;

DECLARE_MULTICAST_DELEGATE(FSpecialKeyPressed);

UCLASS(Blueprintable, Meta=(BlueprintSpawnableComponent))
class UBattleHeroComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()
public:
	UBattleHeroComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintPure, Category="Battle|Hero")
	static UBattleHeroComponent* FindHeroComponent(const AActor* Actor) {return (Actor ? Actor->FindComponentByClass<UBattleHeroComponent>() : nullptr);}
	
	static const FName NAME_ActorFeatureName;

	// GameFrameworkComponentManager Extension Custom Event Name
	static const FName NAME_BindInputsNow;

	// PawnComponent Interface
	virtual void OnRegister() final;
	virtual void BeginPlay() final;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) final;

	// IGameFrameworkInitStateInterface Interface
	virtual FName GetFeatureName() const final {return NAME_ActorFeatureName;}
	virtual void CheckDefaultInitialization() final;
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) final;
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const final;
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) final;

	// Camera
	TSubclassOf<UBattleCameraMode> DetermineCameraMode() const;
	
	UFUNCTION(Client, Reliable)
	void SetSpecialCameraMode(TSubclassOf<UBattleCameraMode> CameraMode, const FBattleCameraModeInitData& InitData);
	
	UFUNCTION(Client, Reliable)
	void ClearSpecialCameraMode();
	
	UBattleCameraSequencePlayer* GetCameraSequencePlayer() const
	{
		return CameraSequencePlayer;
	}



	// Input
	void InitilizePlayerInput(UInputComponent* PlayerInputComponent);
	void ChangePlayerInput(UInputComponent* PlayerInputComponent);
	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_LookMouse(const FInputActionValue& InputActionValue);
	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);
	void Input_SpecialKeyPressed();

	bool CalcKnockbackVector(const FVector& Direction, float Strength, float ZForce, FVector& OutVector);
	
	UFUNCTION(Server, Reliable)
	void PerformDirectionalMove(FVector Direction, float Strength, float ZForce);

	UFUNCTION(BlueprintCallable)
	void PerformKnockback(FVector Direction, float Strength, float ZForce);
	
	void AdditionalInputConfig(const UBattleInputConfig* InputConfig);
	void RemoveAdditionalInputConfig(const UBattleInputConfig* InputConfig);

	FSpecialKeyPressed SpecialKeyPressed;
	
	void OnUnpossessed();
	
protected:

	void Input_RotationCharacter(const FInputActionValue& InputActionValue);
	
	UPROPERTY(EditAnywhere)
	TArray<FBattleMappableConfigPair> DefaultInputConfigs;

	UPROPERTY()
	TSubclassOf<UBattleCameraMode> SpecialCameraMode;

	UPROPERTY()
	UBattleCameraSequencePlayer* CameraSequencePlayer;

protected:

	UAbilitySystemComponent* CachedASC;

	UPROPERTY(EditAnywhere)
	UAnimMontage* KnockbackMontage;

private:
	
	
	
	
};