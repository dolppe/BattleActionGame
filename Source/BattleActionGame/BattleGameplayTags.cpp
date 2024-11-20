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

	/*
	 * InitState
	 */

	AddTag(InitState_Spawned, "InitState.Spawned", "1: Actor/Component has initially spawned and can be extended");
	AddTag(InitState_DataAvailable, "InitState.DataAvailable","2: All required data has been loaded/replicated and is ready for initialization");
	AddTag(InitState_DataInitialized, "InitState.DataInitialized","3: The available data has initialized for this actor/component, but it is not ready for full gameplay");
	AddTag(InitState_GameplayReady, "InitState.GameplayReady","4: The actor/component is fully ready for active gameplay");


	/*
	*	EnhancedInput Tags
	*/
	AddTag(InputTag_Move, "InputTag.Move", "");
	AddTag(InputTag_Look_Mouse, "InputTag.Look.Mouse", "");

	/*
	 *	Status
	*/

	AddTag(Status_Death, "Status.Death", "");
	AddTag(Status_Death_Dying, "Status.Death.Dying", "");
	AddTag(Status_Death_Dead, "Status.Death.Dead", "");

	AddTag(Status_Attack_Attacking, "Status.Attack.Attacking", "");
	AddTag(Status_Poisoned, "Status.Poisoned", "");
	AddTag(Status_Parry, "Status.Parry", "");
	
	AddTag(Status_KnockBack, "Status.KnockBack", "");
	

	/*
	 *	Comboat
	 */

	AddTag(Combat_Attack_Event_Start, "Combat.Attack.Event.Start", "");
	AddTag(Combat_Attack_Event_End, "Combat.Attack.Event.End", "");

	/*
	 * GameplayEffectData
	 */

	AddTag(GameplayEffect_Data_AttackRate, "GameplayEffect.Data.AttackRate", "");

	AddTag(Gameplay_Breakable_LeftLeg, "Gameplay.Breakable.LeftLeg", "");
	AddTag(Gameplay_Breakable_RightLeg, "Gameplay.Breakable.RightLeg", "");
	
	
}
