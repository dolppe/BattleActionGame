#include "AnimNotify_PlaySoundAtCharacterLocation.h"

#include "Kismet/GameplayStatics.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AnimNotify_PlaySoundAtCharacterLocation)


void UAnimNotify_PlaySoundAtCharacterLocation::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (Sound && MeshComp)
	{
		FVector Location = MeshComp->GetComponentLocation() + LocationOffset;
		UGameplayStatics::PlaySoundAtLocation(MeshComp->GetWorld(), Sound, Location, VolumeMultiplier);
	}
}