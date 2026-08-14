#pragma once

#include "Commandlets/Commandlet.h"
#include "BattleAssetValidationCommandlet.generated.h"

UCLASS()
class UBattleAssetValidationCommandlet : public UCommandlet
{
	GENERATED_BODY()
public:
	virtual int32 Main(const FString& Params) override;
	
};
