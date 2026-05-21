#pragma once

#include "AIController.h"
#include "BattleActionGame/Combat/BattleGameplayAbility_Attack_Parent.h"
#include "BattlePlayerAIController.generated.h"

class UBattleAbilitySystemComponent;
class ABattleEnemyCharacter;

UENUM()
enum class EPlayerAIActionType
{
	None,
	SelectBoss,
	MoveToBoss,
	AttackEnemy,
	AvoidAttack,	
};

UCLASS()
class ABattlePlayerAIController : public AAIController 
{
	GENERATED_BODY()
public:
	
	ABattlePlayerAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	void SetAttackGA(TSubclassOf<UBattleGameplayAbility_Attack_Parent> InComboGA, TSubclassOf<UBattleGameplayAbility_Attack_Parent> InStrongGA)
	{
		GA_ComboAttack = InComboGA;
		GA_StrongAttack = InStrongGA;
	}
	
	void StartStrongAttack();
	
protected:
	
	virtual void TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction) override;
	
	bool CanThink();
	EPlayerAIActionType Think();
	
	void SelectBoss();
	void MoveToBoss();
	void AttackToBoss();
	void AvoidAttack();

	
	virtual void OnPossess(APawn* InPawn) override;
	
	UBattleAbilitySystemComponent* CharacterASC = nullptr;
	
	EPlayerAIActionType LastAction = EPlayerAIActionType::None;
	ABattleEnemyCharacter* SelectedBoss = nullptr;
	ABattleEnemyCharacter* CurMoveToBoss = nullptr;
	
	FGameplayAbilitySpec* ComboAbilitySpec = nullptr;
	FGameplayAbilitySpec* ComboStrongAbilitySpec = nullptr;
	
	TSubclassOf<UBattleGameplayAbility_Attack_Parent> GA_ComboAttack;
	TSubclassOf<UBattleGameplayAbility_Attack_Parent> GA_StrongAttack;
	
	float ThinkPeriod = 0.3f;
	float LastThinkTime = -1000.f;
	float AllowedAttackRange = 1100.f;
	float AllowedMoveRange = 400.f;
	
};


