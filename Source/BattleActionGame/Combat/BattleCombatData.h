#pragma once

#include "Engine/DataAsset.h"
#include "BattleCombatData.generated.h"

class UNiagaraSystem;

USTRUCT(BlueprintType)
struct FAttackData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Attack)
	FString MontageSectionName = TEXT("Default");

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Attack)
	FString StartSocketName = TEXT("weapon");

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Attack)
	float AttackRadius = 10.f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Attack)
	TObjectPtr<UAnimMontage> Montage;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Attack)
	TEnumAsByte<ECollisionChannel> CollisionChannel;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Attack)
	USoundBase* AttackSound;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Attack)
	USoundBase* HitSound;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Attack)
	UNiagaraSystem* HitEffect;
	
	
};

USTRUCT()
struct FHitCheckAttack : public FAttackData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category=HitCheck)
	float AttackRange;

	UPROPERTY(EditAnywhere, Category=HitCheck)
	float AttackRate = 1.0f;
	
};

USTRUCT()
struct FSingleAttack : public FAttackData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=SingleAttack)
	FString EndSocketName = TEXT("weapon_end");
	

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=SingleAttack)
	float AttackRate = 1.0f;
	
	
};

USTRUCT()
struct FComboAttack : public FAttackData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category=ComboAttack)
	FString EndSocketName = TEXT("weapon_end");
	
	UPROPERTY(EditAnywhere, Category=ComboAttack)
	uint8 MaxComboCount;

	UPROPERTY(EditAnywhere, Category=ComboAttack)
	float FrameRate = 30;

	UPROPERTY(EditAnywhere, Category=ComboAttack)
	TArray<float> AllowInputFrameCount;

	UPROPERTY(EditAnywhere, Category=ComboAttack)
	TArray<float> AttackRate;
	
	
};


UCLASS(BlueprintType, Const)
class BATTLEACTIONGAME_API UBattleCombatData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:

	UBattleCombatData();

	UPROPERTY(EditAnywhere, Category=SingleAttack)
	TArray<FSingleAttack> SingleAttacks;

	UPROPERTY(EditAnywhere, Category=HitCheckAttack)
	TArray<FHitCheckAttack> HitCheckAttacks;
	
	UPROPERTY(EditAnywhere, Category=ComboAttack)
	TArray<FComboAttack> ComboAttacks;
	
	
};

