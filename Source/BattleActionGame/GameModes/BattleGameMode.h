#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BattleGameMode.generated.h"

class UBattlePawnData;
class UBattleExperienceDefinition;


UCLASS()
class ABattleGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	ABattleGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/*
	 * AGameModeBase Interface
	 */
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void InitGameState() override;

	// Experience의 PawnData을 적용하기 위해 그 전에 Pawn Spawn, Possess 막음.
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	// PawnData를 활용해서 실제로 Spawn되도록 재정의
	virtual APawn* SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) override;

	// PawnData나 Experience의 DefaultPawnData를 리턴.
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	
	virtual bool PlayerCanRestart_Implementation(APlayerController* Player) override;

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	/*
	* Member Methods
	*/
	
	UFUNCTION(BlueprintCallable)
	void RequestPlayerRestartNextFrame(AController* Controller, bool bForceReset = false);

	void HandleMatchAssignmentIfNotExpectingOne();
	void OnMatchAssignmentGiven(FPrimaryAssetId ExperienceId);
	bool IsExperienceLoaded() const;
	const UBattlePawnData* GetPawnDataForController(const AController* InController) const;
	void OnExperienceLoaded(const UBattleExperienceDefinition* CurrentExperience);

	virtual bool ControllerCanRestart(AController* Controller);

	void TryMapLoad();
	void GoToMap();
		
};
