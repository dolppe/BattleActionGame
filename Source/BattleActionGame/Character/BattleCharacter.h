#pragma once
#include "ModularCharacter.h"

#include "BattleCharacter.generated.h"

UCLASS()
class ABattleCharacter : public AModularCharacter
{
	GENERATED_BODY()
public:
	ABattleCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	
};