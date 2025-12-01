#include "HitReactionTable.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(HitReactionTable)

bool UHitReactionTable::GetReactionData(EStrikeType StrikeType, EPhysicalSurface SurfaceType,
	FHitReactionData& OutHitReactionData)
{
	FReactionKey ReactionKey{StrikeType, SurfaceType};
	
	if (const FHitReactionData* Found = ReactionMap.Find(ReactionKey))
	{
		OutHitReactionData = *Found;
		return true;
	}

	ReactionKey.SurfaceType = EPhysicalSurface::SurfaceType_Default;

	if (const FHitReactionData* Found = ReactionMap.Find(ReactionKey))
	{
		OutHitReactionData = *Found;
		return true;
	}

	return false;
	
}
