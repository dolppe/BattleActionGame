	#include "BattleAbilitySet.h"
#include "Abilities/BattleGameplayAbility.h"
#include "BattleAbilitySystemComponent.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleAbilitySet)

void FBattleAbilitySet_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)
{
	if (Handle.IsValid())
	{
		AbilitySpecHandles.Add(Handle);
	}
}

void FBattleAbilitySet_GrantedHandles::AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle)
{
	if (Handle.IsValid())
	{
		GameplayEffectHandles.Add(Handle);
	}
}

void FBattleAbilitySet_GrantedHandles::AddAttributeSet(UAttributeSet* Set)
{
		GrantedAttributeSets.Add(Set);
}

void FBattleAbilitySet_GrantedHandles::TakeFromAbilitySystem(UBattleAbilitySystemComponent* BattleASC)
{
	check(BattleASC);
	
	if (!BattleASC->IsOwnerActorAuthoritative())
	{
		return;
	}

	for (const FGameplayAbilitySpecHandle& Handle : AbilitySpecHandles)
	{
		if (Handle.IsValid())
		{
			BattleASC->ClearAbility(Handle);
		}
	}


	AbilitySpecHandles.Reset();	
	
}

UBattleAbilitySet::UBattleAbilitySet(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

PRAGMA_DISABLE_OPTIMIZATION

void UBattleAbilitySet::GiveToAbilitySystem(UBattleAbilitySystemComponent* BattleASC,
	FBattleAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject) const
{
	check(BattleASC);

	if (!BattleASC->IsOwnerActorAuthoritative())
	{
		return;
	}

	// GA
	for (int32 AbilityIndex = 0; AbilityIndex < GrantedGameplayAbilities.Num(); ++ AbilityIndex)
	{
		const FBattleAbilitySet_GameplayAbility& AbilityToGrant = GrantedGameplayAbilities[AbilityIndex];
		if (!IsValid(AbilityToGrant.Ability))
		{
			continue;
		}

		UBattleGameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<UBattleGameplayAbility>();

		FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToGrant.AbilityLevel);
		AbilitySpec.SourceObject = SourceObject;
		AbilitySpec.DynamicAbilityTags.AddTag(AbilityToGrant.InputTag);

		const FGameplayAbilitySpecHandle AbilitySpecHandle = BattleASC->GiveAbility(AbilitySpec);
		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAbilitySpecHandle(AbilitySpecHandle);
		}
	}

	// GE
	for (int32 EffectIndex = 0; EffectIndex < GrantedGameplayEffects.Num(); ++EffectIndex)
	{
		const FBattleAbilitySet_GameplayEffect& EffectToGrant = GrantedGameplayEffects[EffectIndex];

		if (!IsValid(EffectToGrant.GameplayEffect))
		{
			continue;
		}

		const UGameplayEffect* GameplayEffect = EffectToGrant.GameplayEffect->GetDefaultObject<UGameplayEffect>();
		const FActiveGameplayEffectHandle GameplayEffectHandle = BattleASC->ApplyGameplayEffectToSelf(GameplayEffect, EffectToGrant.EffectLevel, BattleASC->MakeEffectContext());

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddGameplayEffectHandle(GameplayEffectHandle);
		}
	}

	
	// Attribute
	for (int32 SetIndex = 0; SetIndex < GrantedAttributes.Num(); ++SetIndex)
	{
		const FBattleAbilitySet_AttributeSet& SetToGrant = GrantedAttributes[SetIndex];

		if (!IsValid(SetToGrant.AttributeSet))
		{
			continue;
		}

		const UAttributeSet* ConstAttributeSet = BattleASC->GetAttributeSet(SetToGrant.AttributeSet);

		if (ConstAttributeSet)
		{
			UDataTable* InitData = SetToGrant.InitializationData.LoadSynchronous();
			if (InitData)
			{
				UAttributeSet* AttributeSet = const_cast<UAttributeSet*>(ConstAttributeSet);
				AttributeSet->InitFromMetaDataTable(InitData);
			}
		}

		else
		{
			UAttributeSet* NewSet = NewObject<UAttributeSet>(BattleASC->GetOwner(), SetToGrant.AttributeSet);
			if (!SetToGrant.InitializationData.IsNull())
			{
				UDataTable* InitData = SetToGrant.InitializationData.LoadSynchronous();
				if (InitData)
				{
					NewSet->InitFromMetaDataTable(InitData);
				}
			}
			
			BattleASC->AddAttributeSetSubobject(NewSet);

			if (OutGrantedHandles)
			{
				OutGrantedHandles->AddAttributeSet(NewSet);
			}
		}
		

	}
	
	
}

PRAGMA_ENABLE_OPTIMIZATION
