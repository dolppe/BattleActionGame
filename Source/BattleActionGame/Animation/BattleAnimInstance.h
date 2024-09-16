#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GameplayEffectTypes.h"
#include "BattleAnimInstance.generated.h"

class UAbilitySystemComponent;
class ACharacter;
class UBattleCharacterMovementComponent;

UCLASS()
class UBattleAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:

	UBattleAnimInstance(const FObjectInitializer& ObjectInitializer);

	virtual void InitializeWithAbilitySystem(UAbilitySystemComponent* ASC);

protected:

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
protected:

	UPROPERTY(EditDefaultsOnly, Category = "GameplayTags")
	FGameplayTagBlueprintPropertyMap GameplayTagPropertyMap;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Character)
	TObjectPtr<ACharacter> Owner;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Character)
	TObjectPtr<UBattleCharacterMovementComponent> CharacterMovement;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Character)
	FVector CharacterVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Character)
	float CharacterSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Character)
	uint8 bIsIdle : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Character)
	uint8 bIsFalling : 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Character)
	uint8 bIsJumping : 1;	
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Character)
	float RunThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Character)
	float JumpingThreshold;
	
	
	
	
	
};

