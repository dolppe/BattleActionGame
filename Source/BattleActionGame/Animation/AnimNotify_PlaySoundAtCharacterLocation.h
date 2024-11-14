#pragma once

#include "AnimNotify_PlaySoundAtCharacterLocation.generated.h"

UCLASS()
class BATTLEACTIONGAME_API UAnimNotify_PlaySoundAtCharacterLocation : public UAnimNotify
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* Sound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	FVector LocationOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	float VolumeMultiplier = 1.0f;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};