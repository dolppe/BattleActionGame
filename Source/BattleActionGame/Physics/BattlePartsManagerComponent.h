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
	None,
	Damage,
	Destroy,
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
	EPartEventType GetNextPartEventType();
	
	
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
	
};



UCLASS()
class UBattlePartsManagerComponent : public UPawnComponent
{
	GENERATED_BODY()
public:

	UBattlePartsManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable)
	void HandleDamagedToPart(const FGameplayTag& PartTag, const FVector& HitDirection);
	
	UFUNCTION(BlueprintCallable)
	EPhysicalSurface GetSurfaceStateFromPart(const FGameplayTag& PartTag);

	UFUNCTION(BlueprintCallable)
	bool IsPartDestroyed(const FGameplayTag& PartTag);

	UFUNCTION(BlueprintCallable)
	bool WillPartEventTrigger(const FGameplayTag& PartTag);
	
	UFUNCTION(BlueprintCallable)
	bool WillDestroyedPart(const FGameplayTag& PartTag);
	
	void DestroyParts(const FGameplayTag& PartTag);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:

	UFUNCTION()
	void OnRep_DestroyedPartTags();

	UFUNCTION()
	void OnRep_BrokenPartActors();

	UFUNCTION()
	void FinalizeBrokenParts();

	UPROPERTY()
	TArray<ABrokenPartActor*> PendingBrokenParts;
	
	UPROPERTY(EditAnywhere)
	TEnumAsByte<EPhysicalSurface> DefaultSurfaceType = EPhysicalSurface::SurfaceType_Default;
	
	UPROPERTY(EditAnywhere)
	TMap<FGameplayTag, FPartData> PartsData;

	UPROPERTY(ReplicatedUsing=OnRep_DestroyedPartTags)
	TArray<FGameplayTag> DestroyedPartTags;

	UPROPERTY(ReplicatedUsing=OnRep_BrokenPartActors)
	TArray<ABrokenPartActor*> BrokenPartActors;

	TArray<FGameplayTag> AppliedPartTags;


	UPROPERTY(EditAnywhere)
	TSubclassOf<UDestroyedAnimInstance> DestroyedAnimInstance;

private:
	
	void TryApplyDestroyedParts();
	void ApplyDestroyedPart(int32 Index);
	
	void CreateBrokenPartPhysicsAsset(ABrokenPartActor* BrokenPartActor) const;
	void FilterPhysicsAssetForPart(UPhysicsAsset* PhysicsAsset, const TArray<int32>& BodyIndexes, const TArray<FName>& BoneNamesToKeep) const;
	
	ABrokenPartActor* CreateBrokenPartActor(const FName& BoneName);
	void SetBoneInfos(ABrokenPartActor* BrokenPartActor, const FName& BoneName);

	USkeletalMeshComponent* GetOwnerSkeletalMeshComponent() const;
	
};



