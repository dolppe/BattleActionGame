#include "BattleGameplayAbility_UseItem_AttributeBased.h"

#include "AbilitySystemComponent.h"
#include "BattleItemData_AttributeBased.h"
#include "BattleQuickBarComponent.h"
#include "GameplayMessageSubsystem.h"
#include "NativeGameplayTags.h"
#include "NiagaraFunctionLibrary.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionMoveToForce.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "BattleActionGame/BattleGameplayTags.h"
#include "BattleActionGame/BattleLogChannels.h"
#include "BattleActionGame/Character/BattleCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleGameplayAbility_UseItem_AttributeBased)

UBattleGameplayAbility_UseItem_AttributeBased::UBattleGameplayAbility_UseItem_AttributeBased(
	const FObjectInitializer& ObjectInitializer)
		: Super(ObjectInitializer)
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
}

void UBattleGameplayAbility_UseItem_AttributeBased::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                                    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                                    const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UE_LOG(LogBattle, Log, TEXT("ActivateAbility: %s"), GetWorld()->GetNetMode() == NM_Client ? TEXT("Client") : TEXT("Server"));

	if (ABattleCharacterBase* CharacterBase = Cast<ABattleCharacterBase>(GetAvatarActorFromActorInfo()))
	{
		if (UBattleQuickBarComponent* QuickBarComponent = CastChecked<UBattleQuickBarComponent>(CharacterBase->GetController()->GetComponentByClass(UBattleQuickBarComponent::StaticClass())))
		{
			if (!QuickBarComponent->CheckUseActiveSlot(Quantity))
			{
				CancelAbility(Handle, ActorInfo, ActivationInfo, true);
				return;
			}
			
			FBattleItemInfo* ItemInfo = QuickBarComponent->GetActiveSlotItem();

			if (UBattleItemData_AttributeBased* ItemData = Cast<UBattleItemData_AttributeBased>(ItemInfo->ItemDef))
			{
				UAbilityTask_PlayMontageAndWait* PlayMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("PlayMontage"), ItemData->UseAnimation, 1.0f);
				PlayMontage->OnCompleted.AddDynamic(this, &UBattleGameplayAbility_UseItem_AttributeBased::OnCompleted);
				PlayMontage->OnInterrupted.AddDynamic(this, &UBattleGameplayAbility_UseItem_AttributeBased::OnInterrupted);
				PlayMontage->OnBlendOut.AddDynamic(this, &UBattleGameplayAbility_UseItem_AttributeBased::OnBlendOut);
				PlayMontage->ReadyForActivation();
				
				if (CharacterBase->IsLocallyControlled())
				{
					UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(GetWorld());

					UseItemHandle = MessageSubsystem.RegisterListener(FBattleGameplayTags::Get().Combat_UseItem_AttributeBased,this,&ThisClass::StartUseItem);
				}
				if (GetWorld()->GetNetMode() != NM_Client)
				{
					CharacterBase->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
				}
			}
			else
			{
				EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
			}
		}
	}
}

void UBattleGameplayAbility_UseItem_AttributeBased::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	ABattleCharacterBase* Character = Cast<ABattleCharacterBase>(ActorInfo->AvatarActor);

	if (Character->IsLocallyControlled())
	{
		UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(GetWorld());
		MessageSubsystem.UnregisterListener(UseItemHandle);
	}
	if (GetWorld()->GetNetMode() != NM_Client)
	{
		Character->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UBattleGameplayAbility_UseItem_AttributeBased::ServerRPCNotifyUseItem_Implementation()
{
	UE_LOG(LogBattle, Log, TEXT("ServerRPC: %s"), GetWorld()->GetNetMode() == NM_Client ? TEXT("Client") : TEXT("Server"));
	ABattleCharacterBase* CharacterBase = Cast<ABattleCharacterBase>(GetAvatarActorFromActorInfo());
	UBattleQuickBarComponent* QuickBarComponent = CastChecked<UBattleQuickBarComponent>(CharacterBase->GetController()->GetComponentByClass(UBattleQuickBarComponent::StaticClass()));

	if (QuickBarComponent->CheckUseActiveSlot(Quantity))
	{
		UseItem();
	}
}

void UBattleGameplayAbility_UseItem_AttributeBased::UseItem()
{

	UE_LOG(LogBattle, Log, TEXT("UseItem: %s"), GetWorld()->GetNetMode() == NM_Client ? TEXT("Client") : TEXT("Server"));
	ABattleCharacterBase* CharacterBase = Cast<ABattleCharacterBase>(GetAvatarActorFromActorInfo());
	UBattleQuickBarComponent* QuickBarComponent = CastChecked<UBattleQuickBarComponent>(CharacterBase->GetController()->GetComponentByClass(UBattleQuickBarComponent::StaticClass()));

	FBattleItemInfo* ItemInfo = QuickBarComponent->GetActiveSlotItem();
	
	UAbilitySystemComponent* ASC = CharacterBase->GetAbilitySystemComponent();
	UBattleItemData_AttributeBased* ItemData = Cast<UBattleItemData_AttributeBased>(ItemInfo->ItemDef);
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
			
			//DurationSpecHandle.Data->SetDuration(AttData.Duration, false);
			DurationSpecHandle.Data->SetSetByCallerMagnitude(FBattleGameplayTags::Get().GameplayEffect_Caller_Duration, AttData.Duration);

			ASC->ApplyGameplayEffectSpecToSelf(*DurationSpecHandle.Data.Get());
		}
	}

	QuickBarComponent->UseItemQuantity(Quantity);

	OnUsedItem();
	
	// FVector Location = CharacterBase->GetMesh()->GetSocketLocation(TEXT("hand_l"));
	// if (ItemData->ItemSound)
	// {
	// 	float VolumeMultiplier = 0.5f;
	// 	UGameplayStatics::PlaySoundAtLocation(GetWorld(), ItemData->ItemSound, Location, VolumeMultiplier);
	// }
	// if (ItemData->ItemEffect)
	// {
	// 	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ItemData->ItemEffect, Location);
	// }
	
}

void UBattleGameplayAbility_UseItem_AttributeBased::StartUseItem(FGameplayTag Channel,
                                                                 const FBattleVerbMessage& Notification)
{
	if (!IsLocallyControlled())
	{
		return;
	}
	
	UE_LOG(LogBattle, Log, TEXT("StartUseItem: %s"), GetWorld()->GetNetMode() == NM_Client ? TEXT("Client") : TEXT("Server"));
	if (GetWorld()->GetNetMode() == NM_Client)
	{
		ServerRPCNotifyUseItem();
	}
	else
	{
		UseItem();	
	}
}

void UBattleGameplayAbility_UseItem_AttributeBased::OnCompleted()
{
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}

void UBattleGameplayAbility_UseItem_AttributeBased::OnBlendOut()
{
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}

void UBattleGameplayAbility_UseItem_AttributeBased::OnInterrupted()
{
	bool bReplicatedEndAbility = true;
	bool bWasCancelled = true;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicatedEndAbility, bWasCancelled);
}
