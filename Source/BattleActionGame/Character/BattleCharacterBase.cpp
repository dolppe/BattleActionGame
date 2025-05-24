#include "BattleCharacterBase.h"

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
		BA_DEFAULT_LOG(LogBattle, Log, TEXT("Server Start"));
		MulticastPlayMontage(AnimMontage, InPlayRate, StartSectionName);	
	}
	else
	{
		BA_DEFAULT_LOG(LogBattle, Log, TEXT("Client Start"));
		PlayAnimMontage(AnimMontage,InPlayRate,StartSectionName);
		ServerPlayMontage(AnimMontage,InPlayRate,StartSectionName);
	}
	
}

void ABattleCharacterBase::MulticastPlayMontage_Implementation(UAnimMontage* AnimMontage, float InPlayRate,
	FName StartSectionName)
{
	BA_DEFAULT_LOG(LogBattle, Log, TEXT("Multicast Start"));
	USkeletalMeshComponent* CharacterMesh = GetMesh();
	UAnimInstance * AnimInstance = (CharacterMesh)? CharacterMesh->GetAnimInstance() : nullptr;
	if (AnimInstance->Montage_IsPlaying(AnimMontage))
	{
		return;
	}

	PlayAnimMontage(AnimMontage, InPlayRate, StartSectionName);
}
