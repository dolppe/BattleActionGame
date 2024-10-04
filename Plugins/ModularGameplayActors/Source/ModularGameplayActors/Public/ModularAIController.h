#pragma once

#include "AIController.h"
#include "Engine/EngineTypes.h"
#include "UObject/UObjectGlobals.h"
#include "ModularAIController.generated.h"

class UObject;

UCLASS(Blueprintable)
class MODULARGAMEPLAYACTORS_API AModularAIController : public AAIController
{
	GENERATED_BODY()
public:

	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
};

