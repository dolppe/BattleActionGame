#pragma once

#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_CommitState.generated.h"

class UGameplayEffect;

UCLASS()
class UAnimNotify_CommitState : public UAnimNotify
{
	GENERATED_BODY()
public:

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	void BlockMovementWithRotationTag(USkeletalMeshComponent* MeshComp);

	
};
