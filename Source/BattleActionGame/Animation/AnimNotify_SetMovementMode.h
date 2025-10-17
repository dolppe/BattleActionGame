#pragma once

#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_SetMovementMode.generated.h"

UCLASS()
class UAnimNotify_SetMovementMode : public UAnimNotify
{
	GENERATED_BODY()
public:

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;


	UPROPERTY(EditAnywhere)
	TEnumAsByte<EMovementMode> MovementMode;
	
};