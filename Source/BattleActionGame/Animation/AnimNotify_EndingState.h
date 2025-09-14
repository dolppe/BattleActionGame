#pragma once

#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_EndingState.generated.h"

UCLASS()
class UAnimNotify_EndingState : public UAnimNotify
{
	GENERATED_BODY()
public:

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
};
