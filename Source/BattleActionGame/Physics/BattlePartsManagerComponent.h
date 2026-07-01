#pragma once

#include "GameplayTagContainer.h"
#include "Components/PawnComponent.h"
#include "BattlePartsManagerComponent.generated.h"

class UDestroyedAnimInstance;
class ABattleEnemyCharacter;
class UBattlePartsManagerComponent;
class ABrokenPartActor;

UENUM(BlueprintType)
enum class EPartEventType : uint8
{
	Damage,
	Destroy,
};

USTRUCT()
struct FDestroyedPartsData
{
	GENERATED_BODY()

	FDestroyedPartsData(){};
	
	UPROPERTY()
	FName BoneName = NAME_None;
	UPROPERTY()
	ABrokenPartActor* BrokenActor = nullptr;
	UPROPERTY()
	FVector Impulse = FVector(0);
	UPROPERTY()
	FGameplayTag PartTag;
};



USTRUCT(BlueprintType)
struct FPartEventData
{
	GENERATED_BODY()

	void TriggerEvent(FPartData* PartData, UBattlePartsManagerComponent* PartsManager, FGameplayTag PartTag, const FVector& HitDirection);

	UPROPERTY(EditAnywhere)
	int TriggerHp;
	
	UPROPERTY(EditAnywhere)
	EPartEventType EventType;

	UPROPERTY(EditAnywhere, meta=(EditCondition="EventType == EPartEventType::Damage"))
	TEnumAsByte<EPhysicalSurface> NextSurfaceType;
	
};

USTRUCT(BlueprintType)
struct FPartData
{
	GENERATED_BODY()

	void HandleDamaged(UBattlePartsManagerComponent* PartsManager, FGameplayTag PartTag, const FVector& HitDirection, int DamagedHp = 1);
	bool WillDamageTrigger(int DamagedHp = 1);
	
	UPROPERTY(EditAnywhere)
	int PartHp = 5;

	UPROPERTY(EditAnywhere)
	int PartMaxHp = 5;

	UPROPERTY(EditAnywhere)
	TArray<FPartEventData> PartEvents;
	
	UPROPERTY(EditAnywhere)
	FName RootBoneName;

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

	virtual void InitializeComponent() override;

	UFUNCTION(BlueprintCallable)
	void HandleDamagedToPart(const FGameplayTag& PartTag, const FVector& HitDirection);
	
	UFUNCTION(BlueprintCallable)
	EPhysicalSurface GetSurfaceStateFromPart(const FGameplayTag& PartTag);

	UFUNCTION(BlueprintCallable)
	bool IsPartDestroyed(const FGameplayTag& PartTag);

	UFUNCTION(BlueprintCallable)
	bool WillPartEventTrigger(const FGameplayTag& PartTag);
	
	void DestroyParts(const FGameplayTag& PartTag, const FVector& Impulse);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:

	UFUNCTION()
	void TryDestroyedParts(const FGameplayTag& PartTag, const FVector& Impulse, int Idx);

	UFUNCTION()
	void OnRep_DestroyedPartTags();

	UFUNCTION()
	void OnRep_DestroyedParts();

	UFUNCTION()
	void DetachedFrameDelayed();

	UPROPERTY()
	TArray<FDestroyedPartsData> FrameDelayedDestroyedPartsDatas;
	
	UPROPERTY(EditAnywhere)
	TEnumAsByte<EPhysicalSurface> DefaultSurfaceType = EPhysicalSurface::SurfaceType_Default;
	
	UPROPERTY(EditAnywhere)
	TMap<FGameplayTag, FPartData> PartsData;

	UPROPERTY(ReplicatedUsing=OnRep_DestroyedPartTags)
	TArray<FGameplayTag> DestroyedPartTags;

	UPROPERTY(ReplicatedUsing=OnRep_DestroyedParts)
	TArray<ABrokenPartActor*> DestroyedParts;

	TArray<FGameplayTag> AppliedPartTags;


	UPROPERTY(EditAnywhere)
	TSubclassOf<UDestroyedAnimInstance> DestroyedAnimInstance;


private:

	
	void CreateDestroyedPhysicsAsset(ABrokenPartActor* BrokenPartActor, FName InLimb) const;
	void TerminatePhysicsBodies(UPhysicsAsset* PhysicsAsset, int32 Index) const;
	
	ABrokenPartActor* CreateDestroyedPart(const FName& BoneName, const FGameplayTag& PartTag, const FVector& Impulse);
	
	bool DestroyParts_Internal(const FGameplayTag& PartTag, const FVector& Impulse);

	USkeletalMeshComponent* GetOwnerSkeletalMeshComponent();

private:

	FTimerHandle TempHandle;
	FTimerHandle TestHandle;
	
	
};



