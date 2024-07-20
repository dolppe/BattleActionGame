#pragma once

#include "UObject/Interface.h"
#include "UObject/UObjectGlobals.h"

#include "BattleAbilitySourceInterface.generated.h"

class UObject;
class UPhysicalMaterial;
struct FGameplayTagContainer;

/** Base interface for anything acting as a ability calculation source */
UINTERFACE()
class UBattleAbilitySourceInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class IBattleAbilitySourceInterface
{
	GENERATED_IINTERFACE_BODY()


	virtual float GetPhysicalMaterialAttenuation(const UPhysicalMaterial* PhysicalMaterial, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr) const = 0;
};
