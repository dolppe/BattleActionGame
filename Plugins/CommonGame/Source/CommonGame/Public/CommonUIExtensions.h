#pragma once

#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CommonUIExtensions.generated.h"

class UCommonActivatableWidget;
class ULocalPlayer;

UCLASS()
class COMMONGAME_API UCommonUIExtensions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UCommonUIExtensions() {}

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Global UI Extensions")
	static UCommonActivatableWidget* PushContentToLayer_ForPlayer(const ULocalPlayer* LocalPlayer, FGameplayTag LayerName, TSubclassOf<UCommonActivatableWidget> WidgetClass);
	
};


