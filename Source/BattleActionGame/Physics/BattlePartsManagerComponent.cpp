#include "BattlePartsManagerComponent.h"

#include "BrokenPartActor.h"
#include "SkeletalRenderPublic.h"
#include "BattleActionGame/BattleLogChannels.h"
#include "BattleActionGame/Animation/BattleAnimInstance.h"
#include "BattleActionGame/Animation/DestroyedAnimInstance.h"
#include "BattleActionGame/Enemy/BattleEnemyCharacter.h"
#include "Net/UnrealNetwork.h"
#include "PhysicsEngine/PhysicsConstraintTemplate.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattlePartsManagerComponent)

void FPartEventData::TriggerEvent(FPartData* PartData, UBattlePartsManagerComponent* PartsManager, FGameplayTag PartTag, const FVector& HitDirection)
{
	if (EventType == EPartEventType::Damage)
	{
		PartData->SurfaceState = NextSurfaceType;
	}
	else if (EventType == EPartEventType::Destroy)
	{
		PartsManager->DestroyParts(PartTag);
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

EPartEventType FPartData::GetNextPartEventType()
{
	if (PartEvents.IsValidIndex(PartEventIdx))
	{
		return PartEvents[PartEventIdx].EventType;
	}
	
	return EPartEventType::None;
}

UBattlePartsManagerComponent::UBattlePartsManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
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
	if (AppliedPartTags.Contains(PartTag))
	{
		return true;
	}
	
	return false;
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

bool UBattlePartsManagerComponent::WillDestroyedPart(const FGameplayTag& PartTag)
{
	if (FPartData* PartData = PartsData.Find(PartTag))
	{
		if (PartData->WillDamageTrigger())
		{
			if (PartData->GetNextPartEventType() == EPartEventType::Destroy)
			{
				return true;
			}
		}
	}
	return false;
}

void UBattlePartsManagerComponent::DestroyParts(const FGameplayTag& PartTag)
{
	if (!HasAuthority())
	{
		return;
	}
	if (AppliedPartTags.Contains(PartTag))
	{
		return;
	}
	
	USkeletalMeshComponent* OwnerSkeletalMeshComp = GetOwnerSkeletalMeshComponent();
	if (OwnerSkeletalMeshComp == nullptr 
		|| OwnerSkeletalMeshComp->GetSkeletalMeshAsset() == nullptr 
		|| OwnerSkeletalMeshComp->GetPhysicsAsset() == nullptr)
	{
		return;
	}	

	if (FPartData* PartData = PartsData.Find(PartTag))
	{
		FName BoneName = PartData->RootBoneName;
		ABrokenPartActor* BrokenPartActor = CreateBrokenPartActor(BoneName);

		if (BrokenPartActor == nullptr)
		{
			return;
		}
		
		OwnerSkeletalMeshComp->HideBoneByName(BoneName, PBO_Term);
		
		DestroyedPartTags.Add(PartTag);
		BrokenPartActors.Add(BrokenPartActor);
		AppliedPartTags.Add(PartTag);
	}
}

void UBattlePartsManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, DestroyedPartTags);
	DOREPLIFETIME(ThisClass, BrokenPartActors);
	
}




void UBattlePartsManagerComponent::OnRep_DestroyedPartTags()
{
	TryApplyDestroyedParts();	
}

void UBattlePartsManagerComponent::OnRep_BrokenPartActors()
{
	TryApplyDestroyedParts();
}


void UBattlePartsManagerComponent::FinalizeBrokenParts()
{
	for(ABrokenPartActor* BrokenActor : PendingBrokenParts)
	{
		if (!IsValid(BrokenActor) || !IsValid(BrokenActor->SkeletalMeshComponent))
		{
			continue;
		}
		
		USkeletalMeshComponent* BrokenMeshComp = BrokenActor->SkeletalMeshComponent;
		BrokenActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		
		BrokenMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		BrokenMeshComp->SetCollisionObjectType(ECC_PhysicsBody);
		
		CreateBrokenPartPhysicsAsset(BrokenActor);

		BrokenMeshComp->SetSimulatePhysics(true);
	}

	PendingBrokenParts.Reset();
}



ABrokenPartActor* UBattlePartsManagerComponent::CreateBrokenPartActor(const FName& BoneName)
{
	ABrokenPartActor* BrokenPartActor = GetWorld()->SpawnActor<ABrokenPartActor>();
	if (!BrokenPartActor)
	{
		return nullptr;
	}
	USkeletalMeshComponent* BrokenMeshComp = BrokenPartActor->SkeletalMeshComponent;
	USkeletalMeshComponent* OwnerMeshComp = GetOwnerSkeletalMeshComponent();
	
	SetBoneInfos(BrokenPartActor, BoneName);
	
	BrokenPartActor->AttachToComponent(OwnerMeshComp, FAttachmentTransformRules::SnapToTargetIncludingScale, BoneName);
	BrokenMeshComp->SetSkeletalMesh(OwnerMeshComp->GetSkeletalMeshAsset());
	BrokenMeshComp->SetAnimInstanceClass(DestroyedAnimInstance);
	
	UDestroyedAnimInstance* AnimInstance = Cast<UDestroyedAnimInstance>(BrokenMeshComp->GetAnimInstance());

	check(AnimInstance);
	
	AnimInstance->SetPartsName(BoneName);
	
	PendingBrokenParts.Add(BrokenPartActor);
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::FinalizeBrokenParts);
	
	return BrokenPartActor;
	
}

void UBattlePartsManagerComponent::SetBoneInfos(ABrokenPartActor* BrokenPartActor, const FName& BoneName)
{
	USkeletalMesh* SkeletalMesh = GetOwnerSkeletalMeshComponent()->GetSkeletalMeshAsset();
	const FReferenceSkeleton& RefSkeleton = SkeletalMesh->GetRefSkeleton();
	
	const int32 PartRootBoneIndex = RefSkeleton.FindBoneIndex(BoneName);
	
	check(PartRootBoneIndex != INDEX_NONE)
	
	const int32 NumBones = RefSkeleton.GetNum();

	FBoneInfo PartRootBoneInfo;
	PartRootBoneInfo.BoneIndex = PartRootBoneIndex;
	
	BrokenPartActor->BoneInfos.Add(PartRootBoneInfo);

	for (int32 BoneIndex = PartRootBoneIndex + 1; BoneIndex < NumBones; ++BoneIndex)
	{
		int32 ParentIndex = RefSkeleton.GetParentIndex(BoneIndex);

		while (ParentIndex != INDEX_NONE)
		{
			if (ParentIndex == PartRootBoneIndex)
			{
				FBoneInfo BoneInfo;
				BoneInfo.BoneIndex = BoneIndex;
				BrokenPartActor->BoneInfos.Add(BoneInfo);
				break;
			}
			ParentIndex = RefSkeleton.GetParentIndex(ParentIndex);
		}
	}
}


void UBattlePartsManagerComponent::TryApplyDestroyedParts()
{
	for (int32 Idx = 0; Idx < DestroyedPartTags.Num(); ++Idx)
	{
		// 이미 적용한 부위는 제외
		if (AppliedPartTags.Contains(DestroyedPartTags[Idx]))
		{
			continue;
		}
		
		// 대응하는 절단 부위 Actor 참조가 준비된 경우에만 적용
		if (BrokenPartActors.IsValidIndex(Idx) && BrokenPartActors[Idx] != nullptr)
		{
			ApplyDestroyedPart(Idx);		
		}
	}
}

void UBattlePartsManagerComponent::ApplyDestroyedPart(int32 Index)
{
	USkeletalMeshComponent* OwnerSkeletalMeshComp = GetOwnerSkeletalMeshComponent();
	if (OwnerSkeletalMeshComp == nullptr 
		|| OwnerSkeletalMeshComp->GetSkeletalMeshAsset() == nullptr 
		|| OwnerSkeletalMeshComp->GetPhysicsAsset() == nullptr)
	{
		return;
	}
	
	const FGameplayTag& PartTag = DestroyedPartTags[Index];
	
	if (FPartData* PartData = PartsData.Find(PartTag))
	{
		FName BoneName = PartData->RootBoneName;
		OwnerSkeletalMeshComp->HideBoneByName(BoneName, PBO_Term);
		
		ABrokenPartActor* BrokenPartActor = BrokenPartActors[Index];
		USkeletalMeshComponent* BrokenPartActorMeshComp = BrokenPartActor->SkeletalMeshComponent;

		BrokenPartActorMeshComp->SetSkeletalMesh(OwnerSkeletalMeshComp->GetSkeletalMeshAsset());
		BrokenPartActorMeshComp->SetPhysicsAsset(OwnerSkeletalMeshComp->GetPhysicsAsset());

		BrokenPartActorMeshComp->SetAnimInstanceClass(DestroyedAnimInstance);
		
		UDestroyedAnimInstance* AnimInstance = Cast<UDestroyedAnimInstance>(BrokenPartActor->SkeletalMeshComponent->GetAnimInstance());
		check(AnimInstance);
		AnimInstance->SetPartsName(BoneName);
		
		AppliedPartTags.Add(PartTag);
	}
	
}

void UBattlePartsManagerComponent::CreateBrokenPartPhysicsAsset(ABrokenPartActor* BrokenPartActor) const
{
	check(BrokenPartActor);
	check(BrokenPartActor->SkeletalMeshComponent);
	
	USkeletalMeshComponent* BrokenActorMeshComp = BrokenPartActor->SkeletalMeshComponent;	
	const FReferenceSkeleton& RefSkeleton = BrokenActorMeshComp->GetSkeletalMeshAsset()->GetRefSkeleton();
	UPhysicsAsset* SourcePhysicsAsset = GetOwnerSkeletalMeshComponent()->GetPhysicsAsset();
	
	TArray<int32> BodyIndexesToKeep;
	TArray<FName> BoneNamesToKeep;
	
	// 원본 PhysicsAsset을 보존하고, 절단 부위용으로 수정할 복제본 생성
	UPhysicsAsset* BrokenPartPhysicsAsset = DuplicateObject<UPhysicsAsset>(SourcePhysicsAsset, BrokenActorMeshComp);
	
	for (const FBoneInfo& BoneInfo : BrokenPartActor->BoneInfos)
	{
		FName BoneName = RefSkeleton.GetBoneName(BoneInfo.BoneIndex);
		const int32 BodyIndex = BrokenPartPhysicsAsset->FindBodyIndex(BoneName);
		
		if (BodyIndex == INDEX_NONE)
			continue;
		
		BodyIndexesToKeep.Add(BodyIndex);
		BoneNamesToKeep.Add(BoneName);
	}
	
	FilterPhysicsAssetForPart(BrokenPartPhysicsAsset, BodyIndexesToKeep, BoneNamesToKeep);	
	BrokenActorMeshComp->SetPhysicsAsset(BrokenPartPhysicsAsset);
}

void UBattlePartsManagerComponent::FilterPhysicsAssetForPart(UPhysicsAsset* PhysicsAsset, const TArray<int32>& BodyIndexesToKeep, const TArray<FName>& BoneNamesToKeep) const
{
	check(PhysicsAsset);
	
	// 유지할 Body의 기존 인덱스를 재구성 후 신규 인덱스로 매핑
	TMap<int32,int32> OldToNewBodyIndexes;
	for (int32 NewIdx = 0; NewIdx<BodyIndexesToKeep.Num(); ++NewIdx)
	{
		OldToNewBodyIndexes.Add(BodyIndexesToKeep[NewIdx], NewIdx);
	}
	
	// 두 Body가 모두 유지되는 CollisionDisableTable의 쌍만 남기고, 신규 인덱스로 변환
	TMap<FRigidBodyIndexPair,bool> NewCollisionDisableTable;
	for (const TTuple<FRigidBodyIndexPair,bool>& Pair : PhysicsAsset->CollisionDisableTable)
	{
		const int32* NewBodyIndexA = OldToNewBodyIndexes.Find(Pair.Key.Indices[0]);
		const int32* NewBodyIndexB = OldToNewBodyIndexes.Find(Pair.Key.Indices[1]);
		
		if (NewBodyIndexA && NewBodyIndexB)
		{
			FRigidBodyIndexPair NewKey(*NewBodyIndexA, *NewBodyIndexB);
			NewCollisionDisableTable.Add(NewKey, Pair.Value);
		}
	}
	
	// 새로 만든 CollisionDisableTable 적용
	PhysicsAsset->CollisionDisableTable = NewCollisionDisableTable;
	
	
	// 연결된 두 Body가 모두 유지 대상인 Constraint만 보존
	TArray<UPhysicsConstraintTemplate*> NewConstraintSetup;
	for (UPhysicsConstraintTemplate* Constraint : PhysicsAsset->ConstraintSetup)
	{
		if (BoneNamesToKeep.Contains(Constraint->DefaultInstance.ConstraintBone1) 
			&& BoneNamesToKeep.Contains(Constraint->DefaultInstance.ConstraintBone2))
		{
			NewConstraintSetup.Add(Constraint);
		}
	}
	
	// 필터링한 Constraint 목록 적용
	PhysicsAsset->ConstraintSetup = NewConstraintSetup;
	
	
	TArray<USkeletalBodySetup*> NewSkeletalBodySetups;
	
	for (const int32 BodyIndex : BodyIndexesToKeep)
	{
		NewSkeletalBodySetups.Add(PhysicsAsset->SkeletalBodySetups[BodyIndex]);
	}
	PhysicsAsset->SkeletalBodySetups = NewSkeletalBodySetups;

	PhysicsAsset->UpdateBodySetupIndexMap();
	PhysicsAsset->UpdateBoundsBodiesArray();
}





USkeletalMeshComponent* UBattlePartsManagerComponent::GetOwnerSkeletalMeshComponent() const
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

