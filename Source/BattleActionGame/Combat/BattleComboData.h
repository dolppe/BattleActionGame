#pragma once

#include "Engine/DataAsset.h"
#include "BattleComboData.generated.h"

UCLASS(BlueprintType, Const)
class BATTLEACTIONGAME_API UBattleComboData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:

	UBattleComboData();

	UPROPERTY(EditAnywhere, Category=Combo)
	FString MontageSectionName;

	UPROPERTY(EditAnywhere, Category=Combo)
	uint8 MaxComboCount;

	UPROPERTY(EditAnywhere, Category=Combo)
	float FrameRate;

	UPROPERTY(EditAnywhere, Category=Combo)
	TArray<float> AllowInputFrameCount;

	UPROPERTY(EditAnywhere, Category=Combo)
	FString StartSocketName;

	UPROPERTY(EditAnywhere, Category=Combo)
	FString EndSocketName;

	UPROPERTY(EditAnywhere, Category=Combo)
	float AttackRange;

	UPROPERTY(EditAnywhere, Category=Combo)
	float AttackSweep;
	
	
};

