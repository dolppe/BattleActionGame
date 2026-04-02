#pragma once

#include "CoreMinimal.h"
#include "AnimGraphNode_Base.h"
#include "BattleActionGame/Animation/AnimNode_DestroyedParts.h"
#include "AnimGraphNode_DestroyedParts.generated.h"

UCLASS()
class BATTLEACTIONEDITOR_API UAnimGraphNode_DestroyedParts : public UAnimGraphNode_Base
{
	GENERATED_BODY()
public:

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;

	virtual void CreateOutputPins() override;

	UPROPERTY(EditAnywhere, Category = Settings)
	FAnimNode_DestroyedParts Node;
	
	
};

