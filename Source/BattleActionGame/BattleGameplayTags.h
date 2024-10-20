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


	/*
	 * Combat
	 */

	FGameplayTag Combat_Attack_Event;

	/*
	 * GameplayEffectData
	 */
	
	FGameplayTag GameplayEffect_Data_AttackRate;
	

protected:

	void AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment);
	void AddAllTags(UGameplayTagsManager& Manager);
	
	
private:
	static FBattleGameplayTags GameplayTags;
	
};