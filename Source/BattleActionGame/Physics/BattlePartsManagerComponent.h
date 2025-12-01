#pragma once

#include "GameplayTagContainer.h"
#include "Components/PawnComponent.h"
#include "BattlePartsManagerComponent.generated.h"

class ABattleEnemyCharacter;

UENUM(BlueprintType)
enum class EPartEventType : uint8
{
	Damage,
	Destroy,
};

USTRUCT(BlueprintType)
struct FPartEventData
{
	GENERATED_BODY()

	void TriggerEvent(FPartData* PartData, ABattleEnemyCharacter* MyCharacter, const TArray<FName>& BoneNames);

	UPROPERTY(EditAnywhere)
	int TriggerHp;
	
	UPROPERTY(EditAnywhere)
	EPartEventType EventType;

	UPROPERTY(EditAnywhere, meta=(EditCondition="TriggerType == ETriggerType::Damage"))
	TEnumAsByte<EPhysicalSurface> NextSurfaceType;
	
};

USTRUCT(BlueprintType)
struct FPartData
{
	GENERATED_BODY()

	void HandleDamaged(ABattleEnemyCharacter* MyCharacter, int DamagedHp = 1);
	bool WillDamageTrigger(int DamagedHp = 1);
	
	UPROPERTY(EditAnywhere)
	int PartHp = 5;

	UPROPERTY(EditAnywhere)
	int PartMaxHp = 5;

	UPROPERTY(EditAnywhere)
	TArray<FPartEventData> PartEvents;
	
	UPROPERTY(EditAnywhere)
	TArray<FName> BoneNames;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<EPhysicalSurface> SurfaceState = SurfaceType_Default;

	int PartEventIdx = 0;
	bool bIsDestroyed = false;
	
};



UCLASS()
class UBattlePartsManagerComponent : public UPawnComponent
{
	GENERATED_BODY()
public:

	UBattlePartsManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable)
	void HandleDamagedToPart(FName BoneName, FGameplayTag PartTag);
	
	UFUNCTION(BlueprintCallable)
	EPhysicalSurface GetSurfaceStateFromPart(FGameplayTag PartTag);

	UFUNCTION(BlueprintCallable)
	bool IsPartDestroyed(FGameplayTag PartTag);

	UFUNCTION(BlueprintCallable)
	bool WillPartEventTrigger(FGameplayTag PartTag);

	

protected:

	UPROPERTY(EditAnywhere)
	TEnumAsByte<EPhysicalSurface> DefaultSurfaceType = EPhysicalSurface::SurfaceType_Default;
	
	UPROPERTY(EditAnywhere)
	TMap<FGameplayTag, FPartData> PartsData;

	
	
	
};

