#pragma once

#include "CommonGameInstance.h"
#include "BattleGameInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPrimaryPlayerAdded, ULocalPlayer*, NewPlayer);

UCLASS()
class UBattleGameInstance : public UCommonGameInstance
{
	GENERATED_BODY()
public:
	UBattleGameInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void Init() override;
	virtual void Shutdown() override;

	virtual int32 AddLocalPlayer(ULocalPlayer* NewPlayer, FPlatformUserId UserId) override;

	UPROPERTY(BlueprintAssignable)
	FOnPrimaryPlayerAdded OnPrimaryPlayerAdded;
	
};
