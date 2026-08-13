
#include "BattleActionEditor.h"

#include "BattleAssetValidationSubsystem.h"
#include "ToolMenus.h"

IMPLEMENT_GAME_MODULE(FBattleActionEditorModule, BattleActionEditor)

void FBattleActionEditorModule::StartupModule()
{
	UToolMenus::RegisterStartupCallback(
		FSimpleMulticastDelegate::FDelegate::CreateRaw(
			this,
			&FBattleActionEditorModule::RegisterMenus
		)
	);
}

void FBattleActionEditorModule::ShutdownModule()
{
	UToolMenus::UnRegisterStartupCallback(this);
	UToolMenus::UnregisterOwner(this);	
}

void FBattleActionEditorModule::RegisterMenus()
{
	FToolMenuOwnerScoped OwnerScoped(this);
	
	UToolMenu* Menu =
	UToolMenus::Get()->ExtendMenu(
		"LevelEditor.MainMenu.Tools"
	);
	
	FToolMenuSection& Section =
	Menu->FindOrAddSection(
		"BattleTools"
	);
	
	Section.AddMenuEntry(
	   "ValidateBattleAssets",

	   FText::FromString(
		   TEXT("Validate Battle Assets")
	   ),

	   FText::FromString(
		   TEXT("Validate project assets.")
	   ),

	   FSlateIcon(),

	   FUIAction(
		   FExecuteAction::CreateRaw(
			   this,
			   &FBattleActionEditorModule::OnValidateBattleAssetsClicked
		   )
	   )
   );
}

void FBattleActionEditorModule::OnValidateBattleAssetsClicked()
{
	UE_LOG(
	LogTemp,
	Warning,
	TEXT("Validate Battle Assets Clicked")
	);
	
	UBattleAssetValidationSubsystem* AssetValidateSubsystem = GEditor->GetEditorSubsystem<UBattleAssetValidationSubsystem>();
	
	if (AssetValidateSubsystem)
	{
		AssetValidateSubsystem->ValidateAssets();
	}
	else
	{
		UE_LOG(
		LogTemp,
		Warning,
		TEXT("Not AssetValidateSubsystem")
		);
	}
}
	
