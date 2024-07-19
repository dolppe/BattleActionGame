#include "BattleGameplayTags.h"
#include "GameplayTagsManager.h"

FBattleGameplayTags FBattleGameplayTags::GameplayTags;

void FBattleGameplayTags::InitializeNativeTags()
{
	UGameplayTagsManager& Manager = UGameplayTagsManager::Get();
	GameplayTags.AddAllTags(Manager);
}

void FBattleGameplayTags::AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment)
{
	OutTag = UGameplayTagsManager::Get().AddNativeGameplayTag(FName(TagName), FString(TEXT("(Native)")) + FString(TagComment));
}

void FBattleGameplayTags::AddAllTags(UGameplayTagsManager& Manager)
{
}
