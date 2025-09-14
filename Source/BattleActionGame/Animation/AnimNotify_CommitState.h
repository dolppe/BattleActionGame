#pragma once

#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_CommitState.generated.h"

UCLASS()
class UAnimNotify_CommitState : public UAnimNotify
{
	GENERATED_BODY()
public:

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

private:

	USkeletalMeshComponent* CachedMesh;
	
};
