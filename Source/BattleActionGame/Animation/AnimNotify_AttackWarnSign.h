#pragma once

#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_AttackWarnSign.generated.h"

UCLASS()
class UAnimNotify_AttackWarnSign : public UAnimNotify
{
	GENERATED_BODY()
public:

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
	
};

