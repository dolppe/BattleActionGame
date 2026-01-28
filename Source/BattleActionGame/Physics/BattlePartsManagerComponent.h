#pragma once

#include "GameplayTagContainer.h"
#include "Components/PawnComponent.h"
#include "BattlePartsManagerComponent.generated.h"

class UDestroyedAnimInstance;
class ABattleEnemyCharacter;
class UBattlePartsManagerComponent;

UENUM(BlueprintType)
enum class EPartEventType : uint8
{
	Damage,
	Destroy,
};

USTRUCT()
struct FDismemberedLimbFrameDelay
{
	GENERATED_BODY()

	FDismemberedLimbFrameDelay(){};
	FDismemberedLimbFrameDelay(const FName InName, USkeletalMeshComponent* InMesh)
	: BoneName(InName), SkeletalMeshComponent(InMesh){}
	
	UPROPERTY()
	FName BoneName = NAME_None;
	UPROPERTY()
	USkeletalMeshComponent* SkeletalMeshComponent = nullptr;
	UPROPERTY()
	FVector Impulse = FVector(0);
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

	UPROPERTY(EditAnywhere, meta=(EditCondition="TriggerType == ETriggerType::Damage"))
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

	UFUNCTION(NetMulticast, Unreliable)
	void AddImpulseToPart_Multicast(const FGameplayTag& PartTag, const FVector& Impulse);
	
	UFUNCTION()
	void OnRep_DestroyedPartTags();

	UFUNCTION()
	void DetachedFrameDelayed();

	UPROPERTY()
	TArray<FDismemberedLimbFrameDelay> FrameDelayedDismemberedLimbs;


	UPROPERTY(EditAnywhere)
	TEnumAsByte<EPhysicalSurface> DefaultSurfaceType = EPhysicalSurface::SurfaceType_Default;
	
	UPROPERTY(EditAnywhere)
	TMap<FGameplayTag, FPartData> PartsData;

	UPROPERTY(ReplicatedUsing=OnRep_DestroyedPartTags)
	TArray<FGameplayTag> DestroyedPartTags;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UDestroyedAnimInstance> DestroyedAnimInstance;

private:

	
	void CreateDestroyedPhysicsAsset(USkeletalMeshComponent* Component, FName InLimb) const;
	void TerminatePhysicsBodies(UPhysicsAsset* PhysicsAsset, int32 Index) const;
	

	USkeletalMeshComponent* CreateDestroyedPart(const FName& BoneName);
	
	void TryPendingImpulse(const FGameplayTag& PartTag);

	bool DestroyParts_Internal(const FGameplayTag& PartTag);

	USkeletalMeshComponent* GetOwnerSkeletalMeshComponent();
	
	TMap<FGameplayTag, TWeakObjectPtr<USkeletalMeshComponent>> AppliedDestroyedParts;

	TMap<FGameplayTag, FVector> PendingImpulse;






private:

	FTimerHandle TempHandle;
	FTimerHandle TestHandle;
	
	
};



