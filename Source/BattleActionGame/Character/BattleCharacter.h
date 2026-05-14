#pragma once

#include "AbilitySystemInterface.h"
#include "BattleCharacterBase.h"

#include "BattleCharacter.generated.h"

class ABattleObserverPawn;
class UBattleHeroComponent;
class UGameplayEffect;
class UBattleHealthComponent;
class UBattlePawnExtensionComponent;
class UBattleCameraComponent;
class UBattleCameraMode;
class ABattlePlayerAIController;

UCLASS()
class ABattleCharacter : public ABattleCharacterBase
{
	GENERATED_BODY()
public:
	ABattleCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual UBattleAbilitySystemComponent* GetBattleAbilitySystemComponent() const override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual UBattleHealthComponent* GetHealthComponent() const override;

	UBattleHeroComponent* GetHeroComponent();

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	virtual void OnDeathStarted(AActor* OwningActor);

	UFUNCTION()
	virtual void OnDeathFinished(AActor* OwningActor);

	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnDeathFinished"))
	void K2_OnDeathFinished();

	void DisableMovementAndCollision();
	void DestroyDueToDeath();
	void UninitAndDestroy();

	virtual void Reset() override;

	virtual void LaunchCharacter(FVector LaunchVelocity, bool bXYOverride, bool bZOverride) override;

	void StartCriticalHit(FVector ImpactPoint, ABattleCharacterBase* TargetActor);

	UFUNCTION()
	void EndCriticalHit();

	UFUNCTION(BlueprintCallable)
	FORCEINLINE TSubclassOf<UCameraShakeBase> GetCriticalCamera() const
	{
		return CriticalCameraShake;
	}

	ABattleObserverPawn* GetObserverPawn()
	{
		return ObserverPawn;
	}
	
	void SetObserverPawn(ABattleObserverPawn* NewObserverPawn)
	{
		ObserverPawn = NewObserverPawn;
	}
	
	ABattlePlayerAIController* GetAiController()
	{
		return AIController;
	}
	
	void SetAIController(ABattlePlayerAIController* NewAIController)
	{
		AIController = NewAIController;
	}
	
	virtual void PawnClientRestart() override;
	

protected:
	
	

	void OnAbilitySystemInitialized();
	void OnAbilitySystemUninitialized();
	
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

	UFUNCTION()
	void HandleImpactDamage(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude);



	
private:
	// PawnExtensionComponent는 SubObject로 붙여둠.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Battle|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBattlePawnExtensionComponent> PawnExtComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Battle|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBattleCameraComponent> CameraComponent;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UCameraShakeBase> CriticalCameraShake;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UBattleCameraMode> CriticalCameraMode;

	FTimerHandle CriticalHandle;

	FRotator BeforeRotation;
	
	ABattleObserverPawn* ObserverPawn;
	ABattlePlayerAIController* AIController;

	
};