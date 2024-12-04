#include "BattleGameplayAbility_UseItem_AttributeBased.h"

#include "AbilitySystemComponent.h"
#include "BattleItemData_AttributeBased.h"
#include "BattleQuickBarComponent.h"
#include "NativeGameplayTags.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "BattleActionGame/Character/BattleCharacterBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleGameplayAbility_UseItem_AttributeBased)

UBattleGameplayAbility_UseItem_AttributeBased::UBattleGameplayAbility_UseItem_AttributeBased(
	const FObjectInitializer& ObjectInitializer)
		: Super(ObjectInitializer)
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ServerOnly;
	
	ActivationPolicy = EBattleAbilityActivationPolicy::OnInputTriggered;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UBattleGameplayAbility_UseItem_AttributeBased::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                                    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                                    const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (ABattleCharacterBase* CharacterBase = Cast<ABattleCharacterBase>(GetAvatarActorFromActorInfo()))
	{
		if (UBattleQuickBarComponent* QuickBarComponent = CastChecked<UBattleQuickBarComponent>(CharacterBase->GetController()->GetComponentByClass(UBattleQuickBarComponent::StaticClass())))
		{
			FBattleItemInfo* ItemInfo = QuickBarComponent->GetActiveSlotIndex();

			if (UBattleItemData_AttributeBased* ItemData = Cast<UBattleItemData_AttributeBased>(ItemInfo->ItemDef))
			{
				UAbilitySystemComponent* ASC = CharacterBase->GetAbilitySystemComponent();
				
				for (FAttributeData& AttData : ItemData->AttributeData)
				{
					if (AttData.Duration == 0.0f)
					{
						FGameplayEffectSpecHandle InstanceSpecHandle = ASC->MakeOutgoingSpec(InstanceGameplayEffectClass, 1.0f, ASC->MakeEffectContext());
						for (FAttributeMagData& MagData : AttData.AttributeMag)
						{
							InstanceSpecHandle.Data->SetSetByCallerMagnitude(MagData.AttributeTag, MagData.Magnitude);
						}
						InstanceSpecHandle.Data->Duration = FGameplayEffectConstants::INSTANT_APPLICATION;

						ASC->ApplyGameplayEffectSpecToSelf(*InstanceSpecHandle.Data.Get());
					}
					else
					{
						FGameplayEffectSpecHandle DurationSpecHandle = ASC->MakeOutgoingSpec(DurationGameplayEffectClass, 1.0f, ASC->MakeEffectContext());
						for (FAttributeMagData& MagData : AttData.AttributeMag)
						{
							DurationSpecHandle.Data->SetSetByCallerMagnitude(MagData.AttributeTag, MagData.Magnitude);
						}
						DurationSpecHandle.Data->Duration = AttData.Duration;

						ASC->ApplyGameplayEffectSpecToSelf(*DurationSpecHandle.Data.Get());
					}
				}
			}
		}
	}
}
