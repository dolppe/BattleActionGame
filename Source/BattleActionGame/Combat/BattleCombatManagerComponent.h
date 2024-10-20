#pragma once

#include "BattleCombatData.h"
#include "Components/PawnComponent.h"
#include "Animation/AnimMontage.h"
#include "BattleGameplayAbility_ComboAttack.h"
#include "BattleCombatManagerComponent.generated.h"


UCLASS(Blueprintable, Meta=(BlueprintSpawnableComponent))
class UBattleCombatManagerComponent : public UPawnComponent
{
	GENERATED_BODY()

public:

	UBattleCombatManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	FComboAttack* GetComboData(int idx) const
	{
		return &CombatData->ComboAttacks[idx];
	}
	UAnimMontage* GetComboMontage(int idx) const
	{
		return CombatData->ComboAttacks[idx].Montage;
	}

	FSingleAttack* GetAttackData(int idx) const
	{
		return &CombatData->SingleAttacks[idx];
	}

	UAnimMontage* GetAttackMontage(int idx) const
	{
		return CombatData->SingleAttacks[idx].Montage;
	}

	FHitCheckAttack* GetHitCheckAttackData(int idx) const
	{
		return &CombatData->HitCheckAttacks[idx];
	}

	UAnimMontage* GetHitCheckAttackMontage(int idx) const
	{
		return CombatData->HitCheckAttacks[idx].Montage;
	}

	int GetCurrentComboIndex();

	void SetComboGA(UBattleGameplayAbility_ComboAttack* InComboAttack);


private:
	
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UBattleCombatData> CombatData;
	
	UPROPERTY()
	UBattleGameplayAbility_ComboAttack* CurrentCombo = nullptr;
	
	uint8 ComboStep = 0;

};