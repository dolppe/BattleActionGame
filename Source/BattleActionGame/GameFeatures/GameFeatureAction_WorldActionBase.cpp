#include "GameFeatureAction_WorldActionBase.h"

#include "BattleActionGame/BattleLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameFeatureAction_WorldActionBase)


void UGameFeatureAction_WorldActionBase::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
	GameInstanceStartHandles.FindOrAdd(Context) = FWorldDelegates::OnStartGameInstance.AddUObject(this, 
		&UGameFeatureAction_WorldActionBase::HandleGameInstanceStart, FGameFeatureStateChangeContext(Context));

	
	
	for (const FWorldContext& WorldContext : GEngine->GetWorldContexts())
	{
		// 앞서, ExperienceManagerComponent에서 GameFeatureAction을 활성화하면서, Context에 World를 넣어줌.
		// 이를 통해 적용할 대상인지 판단

		UE_LOG(LogBattle, Log, TEXT("%s GFA Activating => %s"), *GetName(), (WorldContext.World()->GetNetMode() == NM_Client? TEXT("Client") : TEXT("Server")));
		
		if (Context.ShouldApplyToWorldContext(WorldContext))
		{
			// WorldActionBase의 Interface인 AddToWorld 호출
			AddToWorld(WorldContext, Context);
		}
	}	
}

void UGameFeatureAction_WorldActionBase::HandleGameInstanceStart(UGameInstance* GameInstance,
	FGameFeatureStateChangeContext ChangeContext)
{
	if (FWorldContext* WorldContext = GameInstance->GetWorldContext())
	{
		if (ChangeContext.ShouldApplyToWorldContext(*WorldContext))
		{
			AddToWorld(*WorldContext, ChangeContext);
		}
	}
}



