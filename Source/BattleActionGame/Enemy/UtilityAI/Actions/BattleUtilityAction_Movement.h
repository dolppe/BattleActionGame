#pragma once

#include "BattleUtilityAction.h"
#include "GameplayAbilitySpec.h"
#include "GameplayTagContainer.h"
#include "Navigation/PathFollowingComponent.h"
#include "BattleUtilityAction_Movement.generated.h"

class UBattleGameplayAbility;
class UAbilitySystemComponent;
class AAIController;

struct FGameplayAbilitySpec;

UCLASS()
class UBAttleUtilityAction_PlayMontage : public UBattleUtilityAction
{
	GENERATED_BODY()
public:
	UBAttleUtilityAction_PlayMontage();
	
	virtual void StartAction() override;
	virtual bool TickAction(float DeltaTime) override;
	virtual void EndAction() override;
	
protected:
	
	UPROPERTY(EditAnywhere)
	UAnimMontage* AnimMontage;
	
	UAnimInstance* AnimInstance;
	
};

UCLASS()
class UBattleUtilityAction_BackAway : public UBattleUtilityAction
{
	GENERATED_BODY()
public:
	UBattleUtilityAction_BackAway();
	
	virtual void StartAction() override;
	virtual bool TickAction(float DeltaTime) override;
	virtual void EndAction() override;
	
protected:
	
	TObjectPtr<AActor> MyCharacter;
	TObjectPtr<AActor> SelectedTarget;
	TObjectPtr<AAIController> AIController;
	
	FVector WantedLocation;
	
	UPROPERTY(EditAnywhere)
	float BackAwayDistance = 1000.f; 
	EPathFollowingRequestResult::Type RequestResult;
	
};

UCLASS()
class UBattleUtilityAction_Strafe : public UBattleUtilityAction
{
	GENERATED_BODY()
public:
	UBattleUtilityAction_Strafe();
	
	virtual void StartAction() override;
	virtual bool TickAction(float DeltaTime) override;
	virtual void EndAction() override;
	
protected:
	
	bool bIsRight = false;
	FVector WantedLocation;
	
	UPROPERTY(EditAnywhere)
	float MaxDistance = 1500.f;
	
	TObjectPtr<AAIController> AIController;
	EPathFollowingRequestResult::Type RequestResult;
};

UCLASS()
class UBattleUtilityAction_AbilityAction : public UBattleUtilityAction
{
	GENERATED_BODY()

public:

	UBattleUtilityAction_AbilityAction();

	/*
	 *	UtilityAction
	 */
	
	virtual void StartAction() override;
	virtual bool TickAction(float DeltaTime) override;
	virtual void EndAction() override;


	virtual void OnEndAbility();

protected:
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UBattleGameplayAbility> AbilityAction;

	FGameplayAbilitySpec* AbilitySpec;

	UAbilitySystemComponent* ASC;
	
};

UCLASS()
class UBattleUtilityAction_MoveToTarget : public UBattleUtilityAction
{
	GENERATED_BODY()

public:

	UBattleUtilityAction_MoveToTarget();

	/*
	 * UtilityAction
	 */

	virtual void StartAction() override;

	virtual void EndAction() override;

	virtual bool TickAction(float DeltaTime) override;
	
protected:


	TObjectPtr<ABattleCharacterBase> SelectedTarget;

	TObjectPtr<ABattleCharacterBase> MyCharacter;

	bool bStartMovement;

};

UCLASS()
class UBattleUtilityAction_TurnToTargetAndStare : public UBattleUtilityAction
{
	GENERATED_BODY()

public:

	UBattleUtilityAction_TurnToTargetAndStare();

	virtual void StartAction() override;

	virtual void EndAction() override;

	virtual bool TickAction(float DeltaTime) override;

protected:
	
	TObjectPtr<ABattleCharacterBase> SelectedTarget;

	TObjectPtr<ABattleCharacterBase> MyCharacter;
	
	FTimerHandle TimerHandle;
	bool bIsEnded;
	
};

UCLASS()
class UBattleUtilityAction_PatrolAround : public UBattleUtilityAction
{
	GENERATED_BODY()

public:

	UBattleUtilityAction_PatrolAround();

	virtual void StartAction() override;

	virtual void EndAction() override;

	virtual bool TickAction(float DeltaTime) override;

protected:

	TObjectPtr<AAIController> AIController;
	EPathFollowingRequestResult::Type RequestResult;
	
};

UCLASS()
class UBattleUtilityAction_MoveToBestSpotWithRushAttack : public UBattleUtilityAction
{
	GENERATED_BODY()

public:

	UBattleUtilityAction_MoveToBestSpotWithRushAttack();

	virtual void StartAction() override;

	virtual bool TickAction(float DeltaTime) override;

protected:

	FGameplayAbilitySpec* AbilitySpec;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UBattleGameplayAbility> RushAttackGA;
	
	TObjectPtr<AAIController> AIController;
	EPathFollowingRequestResult::Type RequestResult;

	bool bAbilityCompleted;
	
};


UCLASS()
class UBattleUtilityAction_MoveToLocationWithGridMap : public UBattleUtilityAction
{
	GENERATED_BODY()

public:

	UBattleUtilityAction_MoveToLocationWithGridMap();

	virtual void StartAction() override;

	virtual void EndAction() override;

	virtual bool TickAction(float DeltaTime) override;

protected:

	TObjectPtr<AAIController> AIController;
	EPathFollowingRequestResult::Type RequestResult;

	
	
};

UCLASS()
class UBattleUtilityAction_MoveToLocation : public UBattleUtilityAction
{
	GENERATED_BODY()

public:

	UBattleUtilityAction_MoveToLocation();

	virtual void StartAction() override;

	virtual void EndAction() override;

	virtual bool TickAction(float DeltaTime) override;

protected:

	TObjectPtr<AAIController> AIController;
	EPathFollowingRequestResult::Type RequestResult;
	virtual FVector GetLocation();
};


UCLASS()
class UBattleUtilityAction_MoveToSelectedSpot : public UBattleUtilityAction_MoveToLocation
{
	GENERATED_BODY()

public:

	UBattleUtilityAction_MoveToSelectedSpot();

protected:

	TObjectPtr<AAIController> AIController;
	EPathFollowingRequestResult::Type RequestResult;
	virtual FVector GetLocation() override;

	UPROPERTY(EditAnywhere)
	FVector SelectedSpot;
};

UCLASS()
class UBattleUtilityAction_MoveToBestSpot : public UBattleUtilityAction_MoveToLocation
{
	GENERATED_BODY()

public:

	UBattleUtilityAction_MoveToBestSpot();

protected:

	virtual FVector GetLocation() override;

	TObjectPtr<AAIController> AIController;
	EPathFollowingRequestResult::Type RequestResult;
	
};

UCLASS()
class UBattleUtilityAction_MoveToBestSpotWithRoar : public UBattleUtilityAction_MoveToBestSpot
{
	GENERATED_BODY()

public:

	UBattleUtilityAction_MoveToBestSpotWithRoar();

	virtual void StartAction() override;

protected:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UBattleGameplayAbility> RoarGA;


	TObjectPtr<AAIController> AIController;
	EPathFollowingRequestResult::Type RequestResult;
	
};




UCLASS()
class UBattleUtilityAction_MoveToRuin : public UBattleUtilityAction_MoveToLocation
{
	GENERATED_BODY()

public:

	UBattleUtilityAction_MoveToRuin();

protected:

	virtual FVector GetLocation() override;

	TObjectPtr<AAIController> AIController;
	EPathFollowingRequestResult::Type RequestResult;
	
};

UCLASS()
class UBattleUtilityAction_MoveToDesert : public UBattleUtilityAction_MoveToLocation
{
	GENERATED_BODY()

public:

	UBattleUtilityAction_MoveToDesert();

protected:

	virtual FVector GetLocation() override;

	TObjectPtr<AAIController> AIController;
	EPathFollowingRequestResult::Type RequestResult;
	
};

UCLASS()
class UBattleUtilityAction_MoveToCave : public UBattleUtilityAction_MoveToLocation
{
	GENERATED_BODY()

public:

	UBattleUtilityAction_MoveToCave();

protected:

	virtual FVector GetLocation() override;

	TObjectPtr<AAIController> AIController;
	EPathFollowingRequestResult::Type RequestResult;
	
};


