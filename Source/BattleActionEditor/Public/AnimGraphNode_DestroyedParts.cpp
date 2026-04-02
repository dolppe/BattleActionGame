#include "AnimGraphNode_DestroyedParts.h"

#include "AnimationGraphSchema.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AnimGraphNode_DestroyedParts)

 #define LOCTEXT_NAMESPACE "AnimGraphNode_DestroyedParts"

FText UAnimGraphNode_DestroyedParts::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("AnimNodeDestroyedParts", "AnimNode DestroyedParts");
}

 void UAnimGraphNode_DestroyedParts::CreateOutputPins()
 {
 	CreatePin(EGPD_Output, UAnimationGraphSchema::PC_Struct, FComponentSpacePoseLink::StaticStruct(), TEXT("ComponentPose"));
 }
 
 #undef LOCTEXT_NAMESPACE
