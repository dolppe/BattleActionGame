#include "BattlePartsManagerComponent.h"

#include "SkeletalRenderPublic.h"
#include "BattleActionGame/BattleLogChannels.h"
#include "BattleActionGame/Animation/BattleAnimInstance.h"
#include "BattleActionGame/Animation/DestroyedAnimInstance.h"
#include "BattleActionGame/Enemy/BattleEnemyCharacter.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattlePartsManagerComponent)

void FPartEventData::TriggerEvent(FPartData* PartData, UBattlePartsManagerComponent* PartsManager, FGameplayTag PartTag, const FVector& HitDirection)
{
	if (EventType == EPartEventType::Damage)
	{
		PartData->SurfaceState = NextSurfaceType;
	}
	else if (EventType == EPartEventType::Destroy)
	{
		PartsManager->DestroyParts(PartTag, HitDirection);
		PartData->bIsDestroyed = true;
	}
	
}

void FPartData::HandleDamaged(UBattlePartsManagerComponent* PartsManager, FGameplayTag PartTag, const FVector& HitDirection, int DamagedHp)
{
	PartHp = FMath::Clamp(PartHp - DamagedHp, 0, PartMaxHp);
	
	if (PartEvents.IsValidIndex(PartEventIdx))
	{
		if (PartEvents[PartEventIdx].TriggerHp >= PartHp)
		{
			PartEvents[PartEventIdx].TriggerEvent(this, PartsManager, PartTag, HitDirection);
			PartEventIdx++;
		}
	}
}

bool FPartData::WillDamageTrigger(int DamagedHp)
{
	if (PartEvents.IsValidIndex(PartEventIdx))
	{
		if (PartEvents[PartEventIdx].TriggerHp >= PartHp - DamagedHp)
		{
			return true;
		}
	}

	return false;
}

UBattlePartsManagerComponent::UBattlePartsManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

void UBattlePartsManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void UBattlePartsManagerComponent::HandleDamagedToPart(const FGameplayTag& PartTag, const FVector& HitDirection)
{
	if (!HasAuthority())
	{
		return;
	}
	if (FPartData* PartData = PartsData.Find(PartTag))
	{

		PartData->HandleDamaged(this, PartTag, HitDirection);
		
	}
}

EPhysicalSurface UBattlePartsManagerComponent::GetSurfaceStateFromPart(const FGameplayTag& PartTag)
{
	if (FPartData* PartData = PartsData.Find(PartTag))
	{
		return PartData->SurfaceState;
	}

	return DefaultSurfaceType;
}

bool UBattlePartsManagerComponent::IsPartDestroyed(const FGameplayTag& PartTag)
{
	if (FPartData* PartData = PartsData.Find(PartTag))
	{
		return PartData->bIsDestroyed;
	}
	
	return true;
}

bool UBattlePartsManagerComponent::WillPartEventTrigger(const FGameplayTag& PartTag)
{
	if (FPartData* PartData = PartsData.Find(PartTag))
	{
		if (PartData->WillDamageTrigger())
		{
			return true;
		}
	}
	return false;
}

void UBattlePartsManagerComponent::DestroyParts(const FGameplayTag& PartTag, const FVector& Impulse)
{
	if (HasAuthority())
	{
		if (!AppliedDestroyedParts.Contains(PartTag))
		{
			if (DestroyParts_Internal(PartTag))
			{
				DestroyedPartTags.Add(PartTag);
				AddImpulseToPart_Multicast(PartTag, Impulse);
			}
		}
	}
}

void UBattlePartsManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, DestroyedPartTags);
}

void UBattlePartsManagerComponent::AddImpulseToPart_Multicast_Implementation(const FGameplayTag& PartTag,
	const FVector& Impulse)
{
	PendingImpulse.Add(PartTag, Impulse);
	TryPendingImpulse(PartTag);
}


void UBattlePartsManagerComponent::OnRep_DestroyedPartTags()
{
	for (FGameplayTag& PartTag : DestroyedPartTags)
	{
		if (!AppliedDestroyedParts.Contains(PartTag))
		{
			DestroyParts_Internal(PartTag);
		}
	}
	
}

PRAGMA_DISABLE_OPTIMIZATION

void UBattlePartsManagerComponent::DetachedFrameDelayed()
{
	for(FDismemberedLimbFrameDelay& Data : FrameDelayedDismemberedLimbs)
	{
		USkeletalMeshComponent* CachedDismemberedLimb = Data.SkeletalMeshComponent;
		CachedDismemberedLimb->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

		CachedDismemberedLimb->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		CachedDismemberedLimb->SetCollisionObjectType(ECC_PhysicsBody);
		
		CreateDestroyedPhysicsAsset(CachedDismemberedLimb, Data.BoneName);

		CachedDismemberedLimb->SetSimulatePhysics(true);

		//CachedDismemberedLimb->AddImpulse(FVector(0.0f,0.0f, 10.f), Data.BoneName, true);
	}

	FrameDelayedDismemberedLimbs.Reset();
}



USkeletalMeshComponent* UBattlePartsManagerComponent::CreateDestroyedPart(const FName& BoneName)
{
	FBodyInstance Body;
	Body.SetObjectType(ECC_WorldStatic);
	Body.SetResponseToAllChannels(ECR_Ignore);
	Body.SetResponseToChannel(ECC_WorldStatic, ECR_Block);
	Body.SetResponseToChannel(ECC_WorldStatic, ECR_Block);
	
	USkeletalMeshComponent* DestroyedPart = NewObject<USkeletalMeshComponent>(GetOwner());
	USkeletalMeshComponent* OwnerMeshComp = GetOwnerSkeletalMeshComponent();
	
	DestroyedPart->AttachToComponent(OwnerMeshComp, FAttachmentTransformRules::SnapToTargetIncludingScale, BoneName);

	DestroyedPart->RegisterComponent();
	
	DestroyedPart->SetSkeletalMesh(OwnerMeshComp->GetSkeletalMeshAsset());
	DestroyedPart->SetPhysicsAsset(OwnerMeshComp->GetPhysicsAsset());

	DestroyedPart->SetAnimInstanceClass(DestroyedAnimInstance);
	
	UDestroyedAnimInstance* AnimInstance = Cast<UDestroyedAnimInstance>(DestroyedPart->GetAnimInstance());

	if (AnimInstance)
	{
		AnimInstance->SetPartsName(BoneName);
	}

	FDismemberedLimbFrameDelay Data;
	Data.SkeletalMeshComponent = DestroyedPart;
	Data.BoneName = BoneName;

	FrameDelayedDismemberedLimbs.Add(Data);
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::DetachedFrameDelayed);

	return DestroyedPart;

	
	
	
}

PRAGMA_ENABLE_OPTIMIZATION

void UBattlePartsManagerComponent::CreateDestroyedPhysicsAsset(USkeletalMeshComponent* Component, FName BoneName) const
{




	const FReferenceSkeleton& RefSkeleton = Component->GetSkeletalMeshAsset()->GetRefSkeleton();

	const int32 ParentBoneIndex = RefSkeleton.FindBoneIndex(BoneName);

	TArray<FName> OutChildBoneNames;
	
	const int32 NumBones = RefSkeleton.GetNum();

	OutChildBoneNames.Add(BoneName);

	for (int32 BoneIndex = ParentBoneIndex + 1; BoneIndex < NumBones; ++BoneIndex)
	{
		int32 ParentIndex = RefSkeleton.GetParentIndex(BoneIndex);

		while (ParentIndex != INDEX_NONE)
		{
			if (ParentIndex == ParentBoneIndex)
			{
				OutChildBoneNames.Add(RefSkeleton.GetBoneName(BoneIndex));
				break;
			}
			ParentIndex = RefSkeleton.GetParentIndex(ParentIndex);
		}
	}


	

	UPhysicsAsset* NewPhysicsAsset = DuplicateObject<UPhysicsAsset>(Component->GetPhysicsAsset(), Component);
	
	TArray<FName> BoneNames;
	Component->GetBoneNames(BoneNames);

	for(const FName Bone : BoneNames)
	{
		const int32 Index = NewPhysicsAsset->FindBodyIndex(Bone);
	
		if(Index == INDEX_NONE) continue;
		if(OutChildBoneNames.Contains(Bone)) continue;

		TerminatePhysicsBodies(NewPhysicsAsset, Index);
	}
	
	Component->SetPhysicsAsset(NewPhysicsAsset, true);
}

void UBattlePartsManagerComponent::TerminatePhysicsBodies(UPhysicsAsset* PhysicsAsset, int32 Index) const
{
	check(PhysicsAsset);
	
	TMap<FRigidBodyIndexPair,bool> NewCDT;
	for(int32 i=1; i< PhysicsAsset->SkeletalBodySetups.Num(); i++)
	{
		for(int32 j=0; j<i; j++)
		{
			FRigidBodyIndexPair Key(j,i);
			
			if( PhysicsAsset->CollisionDisableTable.Find(Key) )
			{
				if(i != Index && j != Index)
				{
					int32 NewI = (i > Index) ? i-1 : i;
					int32 NewJ = (j > Index) ? j-1 : j;

					FRigidBodyIndexPair NewKey(NewJ, NewI);
					NewCDT.Add(NewKey, 0);
				}
			}
		}
	}

	PhysicsAsset->CollisionDisableTable = NewCDT;

	TArray<int32> Constraints;
	PhysicsAsset->BodyFindConstraints(Index, Constraints);

	while(Constraints.Num() > 0)
	{
		PhysicsAsset->ConstraintSetup.RemoveAt(Constraints[0]);
		PhysicsAsset->BodyFindConstraints(Index, Constraints);
	}

	PhysicsAsset->SkeletalBodySetups.RemoveAt(Index);

	PhysicsAsset->UpdateBodySetupIndexMap();
	PhysicsAsset->UpdateBoundsBodiesArray();
}




void UBattlePartsManagerComponent::TryPendingImpulse(const FGameplayTag& PartTag)
{
	if (FPartData* PartData = PartsData.Find(PartTag))
	{
		if (!AppliedDestroyedParts.Find(PartTag))
		{
			return;
		}
		if (USkeletalMeshComponent* SkeletalMeshComponent = AppliedDestroyedParts.Find(PartTag)->Get())
		{
			if (FVector* ImpulseVector = PendingImpulse.Find(PartTag))
			{
				ImpulseVector->Z =200.f;
				SkeletalMeshComponent->AddImpulse(*ImpulseVector);
				SkeletalMeshComponent->AddImpulse(FVector(0.0f,0.0f,5.0f));
				PendingImpulse.Remove(PartTag);
			}
		}
	}
}

bool UBattlePartsManagerComponent::DestroyParts_Internal(const FGameplayTag& PartTag)
{
	if (AppliedDestroyedParts.Contains(PartTag))
	{
		return false;
	}

	USkeletalMeshComponent* SkeletalMeshComponent = GetOwnerSkeletalMeshComponent();
	if (SkeletalMeshComponent == nullptr)
	{
		return false;
	}

	if (FPartData* PartData = PartsData.Find(PartTag))
	{
		FName BoneName = PartData->RootBoneName;

		SkeletalMeshComponent->HideBoneByName(BoneName, PBO_Term);
		SkeletalMeshComponent->BreakConstraint(FVector(0),FVector(0), BoneName);

		USkeletalMeshComponent* DestroyedPart = CreateDestroyedPart(BoneName);

		
		
		AppliedDestroyedParts.Add(PartTag, DestroyedPart);
		TryPendingImpulse(PartTag);
		
		return true;
	}
	
	return false;
}

USkeletalMeshComponent* UBattlePartsManagerComponent::GetOwnerSkeletalMeshComponent()
{
	if (AActor* OwnerActor = GetOwner())
	{
		if (ACharacter* Character = Cast<ACharacter>(OwnerActor))
		{
			
			if (UMeshComponent* Mesh = Character->GetMesh())
			{
				if (USkeletalMeshComponent* SkeletalMeshComponent = Cast<USkeletalMeshComponent>(Mesh))
				{
					return SkeletalMeshComponent;
				}
			}
		}
	}

	return nullptr;
}

