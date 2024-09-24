#pragma once

#include "BattleComboData.h"
#include "Components/PawnComponent.h"
#include "Animation/AnimMontage.h"
#include "BattleCombatManagerComponent.generated.h"

UCLASS(Blueprintable, Meta=(BlueprintSpawnableComponent))
class UBattleCombatManagerComponent : public UPawnComponent
{
	GENERATED_BODY()

public:

	UBattleCombatManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UBattleComboData* GetComboData(int idx)
	{
		return BattleComboData[idx];
	}
	UAnimMontage* GetComboMontage(int idx)
	{
		return ComboAttack[idx];
	}

private:

	/*
	 * 0 => 일반공격
	 * 1 => 강공격
	 */
	
	UPROPERTY(EditDefaultsOnly)
	TArray<UBattleComboData*> BattleComboData;
	
	UPROPERTY(EditDefaultsOnly)
	TArray<UAnimMontage*> ComboAttack;

	uint8 ComboStep = 0;

};