#include "AnimNode_DestroyedParts.h"
#include "Animation/AnimInstanceProxy.h"
#include "BattleActionGame/Physics/BrokenPartActor.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AnimNode_DestroyedParts)

PRAGMA_DISABLE_OPTIMIZATION

void FAnimNode_DestroyedParts::Initialize_AnyThread(const FAnimationInitializeContext& Context)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(Initialize_AnyThread)
	FAnimNode_Base::Initialize_AnyThread(Context);

	ComponentPose.Initialize(Context);
}

void FAnimNode_DestroyedParts::CacheBones_AnyThread(const FAnimationCacheBonesContext& Context)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(CacheBones_AnyThread)
	FAnimNode_Base::CacheBones_AnyThread(Context);
	ComponentPose.CacheBones(Context);
}

void FAnimNode_DestroyedParts::Update_AnyThread(const FAnimationUpdateContext& Context)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(Update_AnyThread);
	Super::Update_AnyThread(Context);

	GetEvaluateGraphExposedInputs().Execute(Context);

	ComponentPose.Update(Context);
}

void FAnimNode_DestroyedParts::EvaluateComponentSpace_AnyThread(FComponentSpacePoseContext& Output)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_ANIMNODE(EvaluateComponentSpace_AnyThread)

	Super::EvaluateComponentSpace_AnyThread(Output);
	// 우선 받은 인풋의 정보를 먼저 Output에 적용
	ComponentPose.EvaluateComponentSpace(Output);

	if (BrokenActor)
	{
		if (BrokenActor->HasAuthority())
		{
			return;
		}
		
		TArray<FBoneTransform> BoneTransforms;
		
		if (MakeBoneTransforms(BoneTransforms, BrokenActor->BoneInfos))
		{
			Output.Pose.LocalBlendCSBoneTransforms(BoneTransforms, 1.0f);	
		}
		
	}
	
}


bool FAnimNode_DestroyedParts::MakeBoneTransforms(TArray<FBoneTransform>& OutBoneTransform, TArray<FBoneInfo>& BoneInfos)
{
	if (BoneInfos.Num() != 0)
	{
		bool bIsChanged = false;

		for (FBoneInfo& BoneInfo : BoneInfos)
		{
			if (BoneInfo.BoneTransform.GetLocation().IsNearlyZero())
			{
				continue;
			}
			bIsChanged = true;
			FBoneTransform BoneTransform;
			BoneTransform.Transform = BoneInfo.BoneTransform;
			BoneTransform.BoneIndex = FCompactPoseBoneIndex(BoneInfo.BoneIndex);
				
			OutBoneTransform.Add(BoneTransform);
		}

		if (bIsChanged)
		{
			return true;	
		}
	}
	
	return false;
	
}

void FAnimNode_DestroyedParts::PreUpdate(const UAnimInstance* InAnimInstance)
{
	FAnimNode_Base::PreUpdate(InAnimInstance);
	
	BrokenActor = nullptr; 

	if (InAnimInstance)
	{
		if (USkeletalMeshComponent* SkeletalMeshComponent = InAnimInstance->GetSkelMeshComponent())
		{
			if (AActor* OwnerActor = SkeletalMeshComponent->GetOwner())
			{
				if (ABrokenPartActor* BrokenPartActor = Cast<ABrokenPartActor>(OwnerActor))
				{
					BrokenActor = BrokenPartActor;
				}
			}
		}
		
	}
	
}

PRAGMA_ENABLE_OPTIMIZATION

