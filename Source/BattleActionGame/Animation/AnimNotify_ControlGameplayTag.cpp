#include "AnimNotify_ControlGameplayTag.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AnimNotify_ControlGameplayTag)

void UAnimNotify_ControlGameplayTag::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp)
	{
		if (IAbilitySystemInterface* AbilitySystemCharacter = Cast<IAbilitySystemInterface>(MeshComp->GetOwner()))
		{
			if (GameplayTag.IsValid())
			{
				UAbilitySystemComponent* ASC = AbilitySystemCharacter->GetAbilitySystemComponent();
				if (ASC)
				{
					if (bAddGameplayTag)
					{
						ASC->AddLooseGameplayTag(GameplayTag);
					}
					else
					{
						ASC->RemoveLooseGameplayTag(GameplayTag);
					}
				}
			}
		}
	}
	
}
