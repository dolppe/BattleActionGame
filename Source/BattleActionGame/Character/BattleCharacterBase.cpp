#include "BattleCharacterBase.h"

#include "BattleActionGame/BattleGameplayTags.h"
#include "BattleActionGame/BattleLogChannels.h"
#include "BattleActionGame/AbilitySystem/BattleAbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleCharacterBase)

ABattleCharacterBase::ABattleCharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UBattleAbilitySystemComponent* ABattleCharacterBase::GetBattleAbilitySystemComponent() const
{
	return NewObject<UBattleAbilitySystemComponent>();
}

UAbilitySystemComponent* ABattleCharacterBase::GetAbilitySystemComponent() const
{
	return NewObject<UAbilitySystemComponent>();
}

UBattleHealthComponent* ABattleCharacterBase::GetHealthComponent() const
{
	return nullptr;
}

void ABattleCharacterBase::NetSetControlRotation(const FRotator& NewRotation)
{
	if (IsLocallyControlled())
	{
		GetController()->SetControlRotation(NewRotation);
	}
	else
	{
		ClientSetControlRotation(NewRotation);
	}
}

void ABattleCharacterBase::ServerPlayMontage_Implementation(UAnimMontage* AnimMontage, float InPlayRate,
                                                            FName StartSectionName)
{
	MulticastPlayMontage(AnimMontage, InPlayRate, StartSectionName);
}

void ABattleCharacterBase::NetPlayMontage(UAnimMontage* AnimMontage, float InPlayRate,
                                                    FName StartSectionName)
{
	if (HasAuthority())
	{
		//BA_DEFAULT_LOG(LogBattle, Log, TEXT("Server Start"));
		MulticastPlayMontage(AnimMontage, InPlayRate, StartSectionName);	
	}
	else
	{
		//BA_DEFAULT_LOG(LogBattle, Log, TEXT("Client Start"));
		PlayAnimMontage(AnimMontage,InPlayRate,StartSectionName);
		ServerPlayMontage(AnimMontage,InPlayRate,StartSectionName);
	}
	
}

void ABattleCharacterBase::MulticastPlayMontage_Implementation(UAnimMontage* AnimMontage, float InPlayRate,
															   FName StartSectionName)
{
	//BA_DEFAULT_LOG(LogBattle, Log, TEXT("Multicast Start"));
	USkeletalMeshComponent* CharacterMesh = GetMesh();
	UAnimInstance * AnimInstance = (CharacterMesh)? CharacterMesh->GetAnimInstance() : nullptr;
	if (AnimInstance->Montage_IsPlaying(AnimMontage))
	{
		return;
	}

	PlayAnimMontage(AnimMontage, InPlayRate, StartSectionName);
}


void ABattleCharacterBase::HandleDamageToPart(FName BoneName, FGameplayTag PartTag)
{
}

void ABattleCharacterBase::NetStopMotion(float StopSeconds, float TimeDilation)
{
	if (HasAuthority())
	{
		MulticastStopMotion(StopSeconds, TimeDilation);
	}
	else
	{

		this->CustomTimeDilation = TimeDilation;

		GetWorldTimerManager().SetTimer(StopMotionHandle,
			[this]()
			{
				this->CustomTimeDilation = 1.0f;
			},
			StopSeconds, false
		);

		ServerStopMotion(StopSeconds, TimeDilation);
	}

}

void ABattleCharacterBase::PerformGroggy()
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		FGameplayTagContainer GameplayTags;
		GameplayTags.AddTag(FBattleGameplayTags::Get().Ability_Type_Reaction_Groggy);
		
		bool bTrySuccess = ASC->TryActivateAbilitiesByTag(GameplayTags);
	}
	
}

void ABattleCharacterBase::PerformPoiseBreak()
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		FGameplayTagContainer GameplayTags;
		GameplayTags.AddTag(FBattleGameplayTags::Get().Ability_Type_Reaction_PoiseBreak);
		
		bool bTrySuccess = ASC->TryActivateAbilitiesByTag(GameplayTags);
	}
}

void ABattleCharacterBase::MulticastStopMotion_Implementation(float StopSeconds, float TimeDilation)
{
	// if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	// {
	// 	if (FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveMontageInstance())
	// 	{
	// 		if (!MontageInstance->IsPlaying())
	// 		{
	// 			return;
	// 		}
	// 		MontageInstance->Pause();
	// 		GetWorldTimerManager().SetTimer(StopMotionHandle,
	// 			[this]()
	// 			{
	// 				this->ResumeMotion();
	// 			},
	// 			StopSeconds, false
	// 		);
	// 	}
	// 	else
	// 	{
	// 		this->CustomTimeDilation = 0.0f;
	//
	// 		GetWorldTimerManager().SetTimer(StopMotionHandle,
	// 			[this]()
	// 			{
	// 				this->CustomTimeDilation = 1.0f;
	// 			},
	// 			StopSeconds, false
	// 		);
	// 	}
	// }

	this->CustomTimeDilation = TimeDilation;

	GetWorldTimerManager().SetTimer(StopMotionHandle,
		[this]()
		{
			this->CustomTimeDilation = 1.0f;
		},
		StopSeconds, false
	);
}

void ABattleCharacterBase::ServerStopMotion_Implementation(float StopSeconds, float TimeDilation)
{
	MulticastStopMotion_Implementation(StopSeconds, TimeDilation);
}

void ABattleCharacterBase::ClientSetControlRotation_Implementation(const FRotator& NewRotation)
{
	if (IsLocallyControlled())
	{
		Controller->SetControlRotation(NewRotation);
	}
}

void ABattleCharacterBase::ResumeMotion()
{
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		if (FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveMontageInstance())
		{
			MontageInstance->SetPlaying(true);	
		}
	}
}

