#pragma once

#include "Containers/UnrealString.h"
#include "Containers/Map.h"
#include "GameplayTagContainer.h"

class UGameplayTagsManager;

struct FBattleGameplayTags
{
	
public:
	static const FBattleGameplayTags& Get() {return GameplayTags;}
	static void InitializeNativeTags();

public:

	/*
	 * InitState
	 */
	
	FGameplayTag InitState_Spawned;
	FGameplayTag InitState_DataAvailable;
	FGameplayTag InitState_DataInitialized;
	FGameplayTag InitState_GameplayReady;


	/*
	 *	Enhanced Input Tags
	 */
	
	FGameplayTag InputTag_Move;
	FGameplayTag InputTag_Look_Mouse;

	/*
	 *	Status
	 */

	FGameplayTag Status_Death;
	FGameplayTag Status_Death_Dying;
	FGameplayTag Status_Death_Dead;
	
	FGameplayTag Status_Attack_Attacking;
	FGameplayTag Status_Poisoned;
	FGameplayTag Status_Parry;

	FGameplayTag Status_KnockBack;


	/*
	 * Combat
	 */

	FGameplayTag Combat_Attack_Event_Start;
	FGameplayTag Combat_Attack_Event_End;
	
	FGameplayTag GameplayEffect_Caller_MaxHp;
	FGameplayTag GameplayEffect_Caller_Stamina;
	FGameplayTag GameplayEffect_Caller_MaxStamina;
	FGameplayTag GameplayEffect_Caller_Healing;
	FGameplayTag GameplayEffect_Caller_Damage;

	FGameplayTag GameplayEffect_Caller_BaseDamage;
	FGameplayTag GameplayEffect_Caller_BaseHeal;
	FGameplayTag GameplayEffect_Caller_AttackPower;
	
	

	/*
	 * GameplayEffectData
	 */
	
	FGameplayTag GameplayEffect_Data_AttackRate;

	/*
	 *
	 */
	FGameplayTag Gameplay_Breakable_LeftLeg;
	FGameplayTag Gameplay_Breakable_RightLeg;
	

protected:

	void AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment);
	void AddAllTags(UGameplayTagsManager& Manager);
	
	
private:
	static FBattleGameplayTags GameplayTags;
	
};