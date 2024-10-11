#include "AnimNotify_SendAttackMessage.h"

#include "AbilitySystemComponent.h"
#include "GameplayMessageSubsystem.h"
#include "BattleActionGame/BattleGameplayTags.h"
#include "BattleActionGame/Character/BattleCharacterBase.h"
#include "BattleActionGame/Messages/BattleVerbMessage.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AnimNotify_SendAttackMessage)

void UAnimNotify_SendAttackMessage::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp)
	{
		if (ABattleCharacterBase* CharacterBase = Cast<ABattleCharacterBase>(MeshComp->GetOwner()))
		{
			FBattleVerbMessage Message;
			Message.Verb = FBattleGameplayTags::Get().Combat_Attack_Event;
			Message.Target = CharacterBase;

			if (CharacterBase->GetWorld())
			{
				UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(CharacterBase->GetWorld());
				MessageSystem.BroadcastMessage(Message.Verb, Message);
			}
		}
		
	}
	
}
