#pragma once

#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_ControlGameplayTag.generated.h"

UCLASS(Blueprintable)
class UAnimNotify_ControlGameplayTag : public UAnimNotify
{
	GENERATED_BODY()
public:

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:

	UPROPERTY(EditAnywhere)
	FGameplayTag GameplayTag;

	UPROPERTY(EditAnywhere)
	bool bAddGameplayTag = true;

	
};

