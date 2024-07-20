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
	

protected:

	void AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment);
	void AddAllTags(UGameplayTagsManager& Manager);
	
	
private:
	static FBattleGameplayTags GameplayTags;
	
};