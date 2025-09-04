#pragma once

#include "AbilitySystemInterface.h"
#include "BattleCharacterBase.h"

#include "BattleCharacter.generated.h"

class UGameplayEffect;
class UBattleHealthComponent;
class UBattlePawnExtensionComponent;
class UBattleCameraComponent;

UCLASS()
class ABattleCharacter : public ABattleCharacterBase
{
	GENERATED_BODY()
public:
	ABattleCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual UBattleAbilitySystemComponent* GetBattleAbilitySystemComponent() const override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual UBattleHealthComponent* GetHealthComponent() const override;

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

protected:

	void OnAbilitySystemInitialized();
	void OnAbilitySystemUninitialized();
	
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

	
private:
	// PawnExtensionComponent는 SubObject로 붙여둠.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Battle|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBattlePawnExtensionComponent> PawnExtComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Battle|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBattleCameraComponent> CameraComponent;

	
};