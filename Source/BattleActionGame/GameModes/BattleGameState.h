#pragma once

#include "AbilitySystemInterface.h"
#include "ModularGameState.h"
#include "BattleActionGame/Player/BattlePlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "BattleGameState.generated.h"

class UBattleExperienceManagerComponent;
class UBattleAbilitySystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerAddedOnlyServer, APlayerState*, PlayerState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerRemovedOnlyServer, APlayerState*, PlayerState);

UENUM()
enum EGameEndingType
{
	NotYet,
	Victory,
	TimeOut,
	DeathCountOut,
};


UCLASS()
class ABattleGameState : public AModularGameStateBase, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:

	ABattleGameState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void PostInitializeComponents() override;
	
	//~IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~End of IAbilitySystemInterface

	UFUNCTION(BlueprintCallable, Category = "Battle|GameState")
	UBattleAbilitySystemComponent* GetBattleAbilitySystemComponent() const { return AbilitySystemComponent; }

	virtual void AddPlayerState(APlayerState* PlayerState) override;
	virtual void RemovePlayerState(APlayerState* PlayerState) override;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerAddedOnlyServer OnPlayerAddedOnlyServer;
	UPROPERTY(BlueprintAssignable)
	FOnPlayerRemovedOnlyServer OnPlayerRemovedOnlyServer;

	UFUNCTION()
	void OnRep_GameReadyNum();

	UFUNCTION(BlueprintCallable)
	void SetGameReadyNum(int InGameReadyNum)
	{
		GameReadyNum = InGameReadyNum;
	}

	UFUNCTION(BlueprintCallable)
	int GetGameReadyNum() const
	{
		return GameReadyNum;
	}

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void SelectBestPlayer();

	UFUNCTION(BlueprintCallable)
	const FCombatStat& GetBestStatValue()
	{
		return BestStatValue;
	}
	

	UFUNCTION(BlueprintCallable)
	TArray<int>& GetBestPlayerIdx()
	{
		return BestPlayerIdx;
	}

	UFUNCTION()
	void OnRep_BestStatValue();

	UFUNCTION()
	void OnRep_BestPlayerIdx();

	UFUNCTION()
	void OnRep_GameEndingType();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void SetGameEnding(EGameEndingType EndingType)
	{
		GameEndingType = EndingType;
	}

	UFUNCTION(BlueprintCallable)
	EGameEndingType GetGameEndingType() const
	{
		return GameEndingType;
	}

private:
	UPROPERTY()
	TObjectPtr<UBattleExperienceManagerComponent> ExperienceManagerComponent;

	UPROPERTY(VisibleAnywhere, Category = "Battle|GameState")
	TObjectPtr<UBattleAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(ReplicatedUsing= OnRep_GameReadyNum)
	int GameReadyNum = 0;

	UPROPERTY(ReplicatedUsing=OnRep_BestStatValue)
	FCombatStat BestStatValue;
	
	UPROPERTY(ReplicatedUsing=OnRep_BestPlayerIdx)
	TArray<int> BestPlayerIdx;

	UPROPERTY(ReplicatedUsing=OnRep_GameEndingType)
	TEnumAsByte<EGameEndingType> GameEndingType = EGameEndingType::NotYet;
	
};
