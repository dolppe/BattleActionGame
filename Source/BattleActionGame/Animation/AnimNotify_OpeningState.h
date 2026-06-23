#pragma once

#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_OpeningState.generated.h"

class UGameplayEffect;

UCLASS()
class UAnimNotify_OpeningState : public UAnimNotify
{
	GENERATED_BODY()
public:

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	void BlockMovementTag(USkeletalMeshComponent* MeshComp);
	
};


