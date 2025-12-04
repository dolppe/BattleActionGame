#pragma once

#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_AllowJustGuard.generated.h"

UCLASS()
class UAnimNotify_AllowJustGuard : public UAnimNotify
{
	GENERATED_BODY()
public:

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
	
};

