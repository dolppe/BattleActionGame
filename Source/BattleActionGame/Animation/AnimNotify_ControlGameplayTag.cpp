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

			UAbilitySystemComponent* ASC = AbilitySystemCharacter->GetAbilitySystemComponent();
			if (ASC)
			{
				TArray<FGameplayTag> TagArrays;
				GameplayTags.GetGameplayTagArray(TagArrays);
				for (const FGameplayTag& Tag : TagArrays)
				{
					if (ASC->HasMatchingGameplayTag(Tag))
					{
						if (!bAddGameplayTag)
						{
							ASC->SetLooseGameplayTagCount(Tag,0);
							
						}
					}
					else
					{
						if (bAddGameplayTag)
						{
							ASC->AddLooseGameplayTag(Tag);
						}
					}
				}
			}
			
		}
	}
	
}
