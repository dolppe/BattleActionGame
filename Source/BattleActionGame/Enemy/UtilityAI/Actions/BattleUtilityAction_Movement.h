#pragma once

#include "BattleUtilityAction.h"
#include "Navigation/PathFollowingComponent.h"
#include "BattleUtilityAction_Movement.generated.h"

class AAIController;

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
class UBattleUtilityAction_TurnToTarget : public UBattleUtilityAction
{
	GENERATED_BODY()

public:

	UBattleUtilityAction_TurnToTarget();

	virtual void StartAction() override;

	virtual void EndAction() override;

	virtual bool TickAction(float DeltaTime) override;

protected:
	
	TObjectPtr<ABattleCharacterBase> SelectedTarget;

	TObjectPtr<ABattleCharacterBase> MyCharacter;
	
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
class UBattleUtilityAction_RunCombat : public UBattleUtilityAction
{
	GENERATED_BODY()

public:

	UBattleUtilityAction_RunCombat();

	virtual void StartAction() override;

	virtual void EndAction() override;

	virtual bool TickAction(float DeltaTime) override;

protected:
		
	TObjectPtr<ABattleCharacterBase> MyCharacter;
	
};

