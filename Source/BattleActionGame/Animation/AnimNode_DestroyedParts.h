#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNodeBase.h"
#include "BattleActionGame/Physics/BrokenPartActor.h"
#include "AnimNode_DestroyedParts.generated.h"

class ABrokenPartActor;

USTRUCT(BlueprintInternalUseOnly)
struct BATTLEACTIONGAME_API FAnimNode_DestroyedParts : public FAnimNode_Base
{
	GENERATED_BODY()
public:

	// Input link
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Links)
	FComponentSpacePoseLink ComponentPose;
	
	bool MakeBoneTransforms(TArray<FBoneTransform>& OutBoneTransform, TArray<FBoneInfo>& BoneInfos);

	// FAnimNode_Base interface
	virtual bool HasPreUpdate() const override { return true; }
	virtual void PreUpdate(const UAnimInstance* InAnimInstance) override;
	virtual void Initialize_AnyThread(const FAnimationInitializeContext& Context) override;
	virtual void CacheBones_AnyThread(const FAnimationCacheBonesContext& Context)  override;
	virtual void Update_AnyThread(const FAnimationUpdateContext& Context) override;
	virtual void EvaluateComponentSpace_AnyThread(FComponentSpacePoseContext& Output) override;
	// End of FAnimNode_Base interface

	TObjectPtr<ABrokenPartActor> BrokenActor;
	
	
};
