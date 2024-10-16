#pragma once

#include "Engine/DataAsset.h"
#include "BattleCombatData.generated.h"

USTRUCT()
struct FHitCheckAttack
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category=HitCheck)
	FString MontageSectionName = TEXT("Default");

	UPROPERTY(EditAnywhere, Category=HitCheck)
	FString StartSocketName = TEXT("weapon");

	UPROPERTY(EditAnywhere, Category=HitCheck)
	float AttackRange;

	UPROPERTY(EditAnywhere, Category=HitCheck)
	float AttackRadius;

	UPROPERTY(EditAnywhere, Category=HitCheck)
	float AttackRate = 1.0f;

	UPROPERTY(EditAnywhere, Category=HitCheck)
	TObjectPtr<UAnimMontage> Montage;

	UPROPERTY(EditAnywhere, Category=HitCheck)
	TEnumAsByte<ECollisionChannel> CollisionChannel;
	
	
};

USTRUCT()
struct FSingleAttack
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category=SingleAttack)
	FString MontageSectionName = TEXT("Default");

	UPROPERTY(EditAnywhere, Category=SingleAttack)
	FString StartSocketName = TEXT("weapon");

	UPROPERTY(EditAnywhere, Category=SingleAttack)
	FString EndSocketName = TEXT("weapon_end");

	UPROPERTY(EditAnywhere, Category=SingleAttack)
	float AttackRadius = 10.f;

	UPROPERTY(EditAnywhere, Category=SingleAttack)
	float AttackRate = 1.0f;

	UPROPERTY(EditAnywhere, Category=SingleAttack)
	TObjectPtr<UAnimMontage> Montage;

	UPROPERTY(EditAnywhere, Category=SingleAttack)
	TEnumAsByte<ECollisionChannel> CollisionChannel;
	
};

USTRUCT()
struct FComboAttack
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category=ComboAttack)
	FString MontageSectionName = TEXT("Default");

	UPROPERTY(EditAnywhere, Category=ComboAttack)
	FString StartSocketName = TEXT("weapon");

	UPROPERTY(EditAnywhere, Category=ComboAttack)
	FString EndSocketName = TEXT("weapon_end");

	UPROPERTY(EditAnywhere, Category=ComboAttack)
	float AttackRadius = 10.f;

	UPROPERTY(EditAnywhere, Category=ComboAttack)
	uint8 MaxComboCount;

	UPROPERTY(EditAnywhere, Category=ComboAttack)
	float FrameRate = 30;

	UPROPERTY(EditAnywhere, Category=ComboAttack)
	TArray<float> AllowInputFrameCount;

	UPROPERTY(EditAnywhere, Category=ComboAttack)
	TArray<float> AttackRate;

	UPROPERTY(EditAnywhere, Category=ComboAttack)
	TObjectPtr<UAnimMontage> Montage;

	UPROPERTY(EditAnywhere, Category=ComboAttack)
	TEnumAsByte<ECollisionChannel> CollisionChannel;
	
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

