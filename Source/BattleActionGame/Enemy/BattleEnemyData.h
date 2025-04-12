#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BattleEnemyData.generated.h"

class UBattleAbilitySet;
class UBattleAbilityTagRelationshipMapping;

UCLASS()
class UBattleEnemyData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:

	UBattleEnemyData(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Battle|Abilities")
	TArray<TObjectPtr<UBattleAbilitySet>> AbilitySets;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Battle|Abilities")
	TObjectPtr<UBattleAbilityTagRelationshipMapping> TagRelationshipMapping;
	
};


