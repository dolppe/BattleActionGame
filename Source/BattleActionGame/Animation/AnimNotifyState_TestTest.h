#pragma once
#include "Animation/AnimNotifies/AnimNotifyState.h"

#include "AnimNotifyState_TestTest.generated.h"


UENUM()
enum class ETestType : uint8
{
	Combo,
	ComboStrong,
	Basic,
	Targeted,
	Spawn,
};

UCLASS(Abstract, Blueprintable, EditInlineNew)
class UTestData : public UObject
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=CollisionData)
	TEnumAsByte<ECollisionChannel> CollisionChannel;

	ETestType CollisionMethodType;
	
};

UCLASS(Blueprintable)
class UTestData_Socket : public UTestData
{
	GENERATED_BODY()

public:

	UTestData_Socket()
	{
		CollisionMethodType = ETestType::Basic;
	}
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=CollisionData)
	FString StartSocketName = TEXT("weapon");

	UPROPERTY(EditAnywhere, Category=CollisionData)
	FString EndSocketName = TEXT("weapon_end");

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=CollisionData)
	float AttackRadius = 10.f;
	
};


UCLASS(Blueprintable)
class UTestData_Direc : public UTestData
{
	GENERATED_BODY()
	
public:

	UTestData_Direc()
	{
		CollisionMethodType = ETestType::Combo;
	}
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=CollisionData)
	FString StartSocketName = TEXT("weapon");

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category=CollisionData)
	float AttackRadius = 10.f;

	UPROPERTY(EditAnywhere, Category=CollisionData)
	float AttackRange = 10.f;

	UPROPERTY(EditAnywhere, Category=CollisionData)
	float AttackYawOffset;
	
	UPROPERTY(EditAnywhere, Category=CollisionData)
	float AttackPitchOffset;
	
};


UCLASS()
class UAnimNotifyState_TestTest : public UAnimNotifyState
{
	GENERATED_BODY()
public:
	
	UAnimNotifyState_TestTest();
	
protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced)
	TArray<UTestData*> TestDatas;
	
	
	TArray<UTestData> TESTSETSETSE;
	
};
