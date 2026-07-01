#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BattleActionGame/Input/BattleMappableConfigPair.h"
#include "BattlePawnData.generated.h"

class UBattleGameplayAbility_Attack_Parent;
class UBattleCameraMode;
class UBattleInputConfig;
class UBattleAbilitySet;
class UBattleAbilityTagRelationshipMapping;

UCLASS()
class UBattlePawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UBattlePawnData(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Battle|Pawn")
	TSubclassOf<APawn> PawnClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Battle|Camera")
	TSubclassOf<UBattleCameraMode> DefaultCameraMode;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Battle|InputConfig")
	TObjectPtr<UBattleInputConfig> InputConfig;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Battle|Abilities")
	TArray<TObjectPtr<UBattleAbilitySet>> AbilitySets;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Battle|Abilities")
	TObjectPtr<UBattleAbilityTagRelationshipMapping> TagRelationshipMapping;
	
	UPROPERTY(EditDefaultsOnly, Category="Battle|Observer")
	TSubclassOf<UBattleCameraMode> ObserverDefaultCameraMode;
	
	UPROPERTY(EditDefaultsOnly, Category="Battle|Observer")
	TObjectPtr<UBattleInputConfig> ObserverInputData;
	
	UPROPERTY(EditDefaultsOnly, Category="Battle|Observer")
	TArray<FBattleMappableConfigPair> ObserverInputConfigs;

	UPROPERTY(EditDefaultsOnly, Category="Battle|AIController")
	TSubclassOf<UBattleGameplayAbility_Attack_Parent> AIComboGA;
	
	UPROPERTY(EditDefaultsOnly, Category="Battle|AIController")
	TSubclassOf<UBattleGameplayAbility_Attack_Parent> AIComboStrongGA;
};

