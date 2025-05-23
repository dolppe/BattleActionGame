#pragma once

#include "Engine/DataAsset.h"
#include "BattleCombatData.generated.h"

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
struct FAttackData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Attack)
	FString MontageSectionName = TEXT("Default");

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Attack)
	TObjectPtr<UAnimMontage> Montage;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Attack)
	USoundBase* AttackSound;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Attack)
	USoundBase* HitSound;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Attack)
	UNiagaraSystem* HitEffect;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Attack)
	TArray<TSubclassOf<UGameplayEffect>> AppliedEffectsToTarget;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=Attack)
	TArray<TSubclassOf<UGameplayEffect>> AppliedEffectsToSelf;

	UPROPERTY(EditAnywhere, Instanced, BlueprintReadOnly, Category = "Attack")
	UAttackCollisionData* CollisionMethod;


	
};

USTRUCT()
struct FBasicAttack : public FAttackData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category=BasicAttack)
	float BaseDamage = 0.0f;
	
	UPROPERTY(EditAnywhere, Category=BasicAttack)
	float AttackRate = 1.0f;

	UPROPERTY(EditAnywhere, Category=BasicAttack)
	float GroggyValue = 10.0f;
	
};

USTRUCT()
struct FComboStrongAttack : public FAttackData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category=ComboStrongAttack)
	float BaseDamage = 0.0f;
	
	UPROPERTY(EditAnywhere, Category=ComboStrongAttack)
	float AttackRate = 1.0f;

	UPROPERTY(EditAnywhere, Category=ComboStrongAttack)
	float GroggyValue = 10.0f;	
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

	UPROPERTY(EditAnywhere, Category=ComboAttack)
	TArray<float> BaseDamage;
	
	UPROPERTY(EditAnywhere, Category=ComboAttack)
	TArray<float> AttackRate;

	UPROPERTY(EditAnywhere, Category=ComboAttack)
	TArray<float> GroggyValue;
	
};

USTRUCT()
struct FTargetedAttack : public FAttackData
{
	GENERATED_BODY()

	FTargetedAttack();

	UPROPERTY(EditAnywhere, Category=BasicAttack)
	float BaseDamage = 0.0f;
	
	UPROPERTY(EditAnywhere, Category=BasicAttack)
	float AttackRate = 1.0f;

	UPROPERTY(EditAnywhere, Category=BasicAttack)
	float GroggyValue = 10.0f;

	
	
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
	TArray<FBasicAttack> BasicAttacks;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=ComboAttack)
	TArray<FComboAttack> ComboAttacks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=ComboStrongAttack)
	TArray<FComboStrongAttack> ComboStrongAttacks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=TargetedAttack)
	TArray<FTargetedAttack> TargetedAttacks;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=SpawnAttack)
	TArray<FSpecialSpawnData> SpawnDatas;
	
};

