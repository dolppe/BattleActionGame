#pragma once

#include "HitReactionTable.h"
#include "Engine/DataAsset.h"
#include "BattleCombatData.generated.h"

class UHitReactionTable;
class UGameplayEffect;
enum class ECollisionMethodType : uint8;
class UNiagaraSystem;


UENUM()
enum class EAttackType : uint8
{
	Combo,
	ComboStrong,
	Basic,
	Targeted,
	Spawn,
};

UCLASS(Abstract, Blueprintable, EditInlineNew)
class UAttackCollisionData : public UObject
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=CollisionData)
	TEnumAsByte<ECollisionChannel> CollisionChannel;

	ECollisionMethodType CollisionMethodType;
	
};

UCLASS(Blueprintable, EditInlineNew)
class UAttackCollisionData_SocketBasedLineTrace : public UAttackCollisionData
{
	GENERATED_BODY()

public:

	UAttackCollisionData_SocketBasedLineTrace();
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=CollisionData)
	FString StartSocketName = TEXT("weapon");

	UPROPERTY(EditAnywhere, Category=CollisionData)
	FString EndSocketName = TEXT("weapon_end");

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=CollisionData)
	float AttackRadius = 10.f;
	
};


UCLASS(Blueprintable, EditInlineNew)
class UAttackCollisionData_DirectionalSweep : public UAttackCollisionData
{
	GENERATED_BODY()
	
public:

	UAttackCollisionData_DirectionalSweep();
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=CollisionData)
	FString StartSocketName = TEXT("weapon");

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=CollisionData)
	float AttackRadius = 10.f;

	UPROPERTY(EditAnywhere, Category=CollisionData)
	float AttackRange = 10.f;

	UPROPERTY(EditAnywhere, Category=CollisionData)
	float AttackRotationOffset;
	
};

UCLASS(Blueprintable, EditInlineNew)
class UAttackCollisionData_CircularAOE : public UAttackCollisionData
{
	GENERATED_BODY()

public:
	
	UAttackCollisionData_CircularAOE();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=CollisionData)
	float AttackRadius = 100.f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=CollisionData)
	int AttackNum = 1;
	
};

USTRUCT(BlueprintType)
struct FAttackWindowData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Attack)
	TArray<TSubclassOf<UGameplayEffect>> AppliedEffectsToTarget;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Attack)
	TArray<TSubclassOf<UGameplayEffect>> AppliedEffectsToSelf;
	
	UPROPERTY(EditAnywhere, Category=Attack)
	float BaseDamage;
	
	UPROPERTY(EditAnywhere, Category=Attack)
	float AttackRate;

	UPROPERTY(EditAnywhere, Category=Attack)
	float GroggyValue;

	UPROPERTY(EditAnywhere, Category=Attack)
	EStrikeType StrikeType = EStrikeType::SwordA;
	
};

USTRUCT(BlueprintType)
struct FAttackData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Attack)
	FString MontageSectionName = TEXT("Default");

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Attack)
	TObjectPtr<UAnimMontage> Montage;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Attack)
	TArray<FAttackWindowData> AttackWindowDatas;
	
	// UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly, Category = "Attack")
	// UAttackCollisionData* CollisionMethod;
	
};




USTRUCT()
struct FComboAttack : public FAttackData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category=ComboAttack)
	uint8 MaxComboCount;

	UPROPERTY(EditAnywhere, Category=ComboAttack)
	float FrameRate = 30;

	UPROPERTY(EditAnywhere, Category=ComboAttack)
	TArray<float> AllowInputFrameCount;


	
};

USTRUCT(BlueprintType)
struct FSpecialSpawnData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Spawn)
	FString MontageSectionName = TEXT("Default");

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Spawn)
	TObjectPtr<UAnimMontage> Montage;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Spawn)
	USoundBase* MontageStartSound;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Spawn)
	TSubclassOf<AActor> SpawnedActor;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Spawn)
	int SpawnActorNum = 1;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Spawn)
	float SpawnRadius = 100.f;
	
};


UCLASS(BlueprintType, Const)
class BATTLEACTIONGAME_API UBattleCombatData : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:

	UBattleCombatData();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=BasicAttack)
	TArray<FAttackData> BasicAttacks;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=ComboAttack)
	TArray<FComboAttack> ComboAttacks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=ComboStrongAttack)
	TArray<FAttackData> ComboStrongAttacks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=TargetedAttack)
	TArray<FAttackData> TargetedAttacks;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=SpawnAttack)
	TArray<FSpecialSpawnData> SpawnDatas;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UHitReactionTable* HitReactionTable;
	
};

