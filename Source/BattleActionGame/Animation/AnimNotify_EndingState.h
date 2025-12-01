#pragma once

#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_EndingState.generated.h"

class UGameplayEffect;

UCLASS()
class UAnimNotify_EndingState : public UAnimNotify
{
	GENERATED_BODY()
public:

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;


protected:

	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> RemoveGE1;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayEffect> RemoveGE2;

private:

	void BlockFuncC(USkeletalMeshComponent* MeshComp);
	void AllowFuncC(USkeletalMeshComponent* MeshComp);
	
};
