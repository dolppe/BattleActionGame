#include "BattlePartsManagerComponent.h"

#include "BrokenPartActor.h"
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
		DestroyParts_Internal(PartTag, Impulse);
	}
}

void UBattlePartsManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, DestroyedPartTags);
	DOREPLIFETIME(ThisClass, DestroyedParts);
	
}

void UBattlePartsManagerComponent::TryDestroyedParts(const FGameplayTag& PartTag, const FVector& Impulse, int Idx)
{
	if (!DestroyedParts.IsValidIndex(Idx))
	{
		return;
	}
	else if (DestroyedParts[Idx] == nullptr)
	{
		return;
	}

	DestroyParts_Internal(PartTag, Impulse);
}


PRAGMA_DISABLE_OPTIMIZATION

void UBattlePartsManagerComponent::OnRep_DestroyedPartTags()
{
	int Idx = 0;
	
	for (FGameplayTag& PartTag : DestroyedPartTags)
	{
		if (!AppliedPartTags.Contains(PartTag))
		{
			TryDestroyedParts(PartTag, FVector(), Idx);
		}
		Idx++;
	}
	
}

void UBattlePartsManagerComponent::OnRep_DestroyedParts()
{
	int Idx = 0;
	
	for (FGameplayTag& PartTag : DestroyedPartTags)
	{
		if (!AppliedPartTags.Contains(PartTag))
		{
			TryDestroyedParts(PartTag, FVector(), Idx);
		}
		Idx++;
	}
}


void UBattlePartsManagerComponent::DetachedFrameDelayed()
{
	for(FDismemberedLimbFrameDelay& Data : FrameDelayedDismemberedLimbs)
	{
		ABrokenPartActor* BrokenActor = Data.BrokenActor;
		USkeletalMeshComponent* BrokenMeshComp = BrokenActor->SkeletalMeshComponent;
		BrokenActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		

		BrokenMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		BrokenMeshComp->SetCollisionObjectType(ECC_PhysicsBody);
		
		CreateDestroyedPhysicsAsset(BrokenActor, Data.BoneName);

		BrokenMeshComp->SetSimulatePhysics(true);
	}

	FrameDelayedDismemberedLimbs.Reset();
}



ABrokenPartActor* UBattlePartsManagerComponent::CreateDestroyedPart(const FName& BoneName, const FGameplayTag& PartTag, const FVector& Impulse)
{
	FBodyInstance Body;
	Body.SetObjectType(ECC_WorldStatic);
	Body.SetResponseToAllChannels(ECR_Ignore);
	Body.SetResponseToChannel(ECC_WorldStatic, ECR_Block);
	Body.SetResponseToChannel(ECC_WorldStatic, ECR_Block);


	ABrokenPartActor* DestroyedPart = GetWorld()->SpawnActor<ABrokenPartActor>();
	USkeletalMeshComponent* OwnerMeshComp = GetOwnerSkeletalMeshComponent();
	const FReferenceSkeleton& RefSkeleton = OwnerMeshComp->GetSkeletalMeshAsset()->GetRefSkeleton();
	
	DestroyedPart->AttachToComponent(OwnerMeshComp, FAttachmentTransformRules::SnapToTargetIncludingScale, BoneName);
	
	DestroyedPart->SkeletalMeshComponent->SetSkeletalMesh(OwnerMeshComp->GetSkeletalMeshAsset());
	DestroyedPart->SkeletalMeshComponent->SetPhysicsAsset(OwnerMeshComp->GetPhysicsAsset());

	DestroyedPart->SkeletalMeshComponent->SetAnimInstanceClass(DestroyedAnimInstance);
	
	UDestroyedAnimInstance* AnimInstance = Cast<UDestroyedAnimInstance>(DestroyedPart->SkeletalMeshComponent->GetAnimInstance());

	if (AnimInstance)
	{
		AnimInstance->SetPartsName(BoneName);
	}

	FDismemberedLimbFrameDelay Data;
	Data.BrokenActor = DestroyedPart;
	Data.BoneName = BoneName;
	Data.PartTag = PartTag;
	Data.Impulse = Impulse;

	FrameDelayedDismemberedLimbs.Add(Data);
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::DetachedFrameDelayed);
	
	const int32 ParentBoneIndex = RefSkeleton.FindBoneIndex(BoneName);

	TArray<FName> OutChildBoneNames;
	
	const int32 NumBones = RefSkeleton.GetNum();

	FBoneInfo RootBoneInfo;

	RootBoneInfo.BoneIndex = 0;

	DestroyedPart->BoneInfos.Add(RootBoneInfo);

	for (int32 BoneIndex = ParentBoneIndex + 1; BoneIndex < NumBones; ++BoneIndex)
	{
		int32 ParentIndex = RefSkeleton.GetParentIndex(BoneIndex);

		while (ParentIndex != INDEX_NONE)
		{
			if (ParentIndex == ParentBoneIndex)
			{
				FBoneInfo BoneTransform;
				BoneTransform.BoneIndex = BoneIndex;
				DestroyedPart->BoneInfos.Add(BoneTransform);
				break;
			}
			ParentIndex = RefSkeleton.GetParentIndex(ParentIndex);
		}
	}

	return DestroyedPart;
	
}

PRAGMA_ENABLE_OPTIMIZATION

void UBattlePartsManagerComponent::CreateDestroyedPhysicsAsset(ABrokenPartActor* BrokenPartActor, FName BoneName) const
{
	USkeletalMeshComponent* Component = BrokenPartActor->SkeletalMeshComponent;
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
	
	BrokenPartActor->SkeletalMeshComponent->SetPhysicsAsset(NewPhysicsAsset);
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


bool UBattlePartsManagerComponent::DestroyParts_Internal(const FGameplayTag& PartTag, const FVector& Impulse)
{
	if (AppliedPartTags.Contains(PartTag))
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

		if (HasAuthority())
		{
			ABrokenPartActor* DestroyedPart = CreateDestroyedPart(BoneName, PartTag, Impulse);

			DestroyedPartTags.Add(PartTag);
			DestroyedParts.Add(DestroyedPart);
			AppliedPartTags.Add(PartTag);
		}
		else
		{
			int Idx = 0;
			for (const FGameplayTag& SelectedPartTag : DestroyedPartTags)
			{
				if (SelectedPartTag == PartTag)
				{
					if (ABrokenPartActor* BrokenPartActor = DestroyedParts[Idx])
					{
						//BrokenPartActor
						USkeletalMeshComponent* OwnerMeshComp = GetOwnerSkeletalMeshComponent();
						USkeletalMeshComponent* BrokenMeshComp = BrokenPartActor->SkeletalMeshComponent;

						BrokenMeshComp->SetSkeletalMesh(OwnerMeshComp->GetSkeletalMeshAsset());
						BrokenMeshComp->SetPhysicsAsset(OwnerMeshComp->GetPhysicsAsset());

						BrokenMeshComp->SetAnimInstanceClass(DestroyedAnimInstance);
						UDestroyedAnimInstance* AnimInstance = Cast<UDestroyedAnimInstance>(BrokenPartActor->SkeletalMeshComponent->GetAnimInstance());
						
						if (AnimInstance)
						{
							AnimInstance->SetPartsName(BoneName);
						}

						//this->CreateDestroyedPhysicsAsset(BrokenPartActor, PartData->RootBoneName);
						
						// FBodyInstance Body;
						// Body.SetObjectType(ECC_WorldStatic);
						// Body.SetResponseToAllChannels(ECR_Ignore);
						// Body.SetResponseToChannel(ECC_WorldStatic, ECR_Block);
						// Body.SetResponseToChannel(ECC_WorldStatic, ECR_Block);
						//
						// BrokenMeshComp->SetCollisionObjectType(Body.GetObjectType());
						// BrokenMeshComp->SetCollisionEnabled(Body.GetCollisionEnabled());
						// BrokenMeshComp->SetCollisionResponseToChannels(Body.GetResponseToChannels());
						// BrokenMeshComp->SetCanEverAffectNavigation(CanEverAffectNavigation());
						//
						// GetWorld()->GetTimerManager().SetTimerForNextTick([this, BrokenMeshComp, BrokenPartActor, PartData]()
						// {
						// 	BrokenMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
						// 	BrokenMeshComp->SetCollisionObjectType(ECC_PhysicsBody);
						//
						// 	this->CreateDestroyedPhysicsAsset(BrokenPartActor, PartData->RootBoneName);
						//
						// 	BrokenMeshComp->SetSimulatePhysics(true);
						// });
					}
					else
					{
						return false;
					}
				}
				Idx++;
			}
			AppliedPartTags.Add(PartTag);
		}
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

