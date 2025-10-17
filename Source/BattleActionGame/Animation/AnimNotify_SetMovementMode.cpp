#include "AnimNotify_SetMovementMode.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(AnimNotify_SetMovementMode)

void UAnimNotify_SetMovementMode::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner()))
	{
		if (UCharacterMovementComponent* MovementComponent = Character->GetCharacterMovement())
		{
			MovementComponent->SetMovementMode(MovementMode);
		}
		
	}

}
