#pragma once

#include "BattleCombatData.h"
#include "Components/PawnComponent.h"
#include "Animation/AnimMontage.h"
#include "BattleGameplayAbility_ComboAttack.h"
#include "BattleCombatManagerComponent.generated.h"

UENUM()
enum class EAttackType : uint8
{
	Single,
	Combo,
	HitCheck,
};


UCLASS(Blueprintable, Meta=(BlueprintSpawnableComponent))
class UBattleCombatManagerComponent : public UPawnComponent
{
	GENERATED_BODY()

public:

	UBattleCombatManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable)
	const FComboAttack& GetComboData(int idx) const
	{
		return CombatData->ComboAttacks[idx];
	}

	UFUNCTION(BlueprintCallable)
	const FSingleAttack& GetAttackData(int idx) const
	{
		return CombatData->SingleAttacks[idx];
	}

	UFUNCTION(BlueprintCallable)
	const FHitCheckAttack& GetHitCheckAttackData(int idx) const
	{
		return CombatData->HitCheckAttacks[idx];
	}

	UFUNCTION(BlueprintCallable)
	UAnimMontage* GetAttackMontage(EAttackType AttackType, int Idx) const
	{
		switch (AttackType)
		{
		case EAttackType::Single:
			return CombatData->SingleAttacks[Idx].Montage;
		case EAttackType::Combo:
			return CombatData->ComboAttacks[Idx].Montage;
		case EAttackType::HitCheck:
			return CombatData->HitCheckAttacks[Idx].Montage;
		}
		return nullptr;
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