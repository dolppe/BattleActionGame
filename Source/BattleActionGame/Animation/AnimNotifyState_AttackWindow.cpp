#include "AnimNotifyState_AttackWindow.h"

#include "GameplayMessageSubsystem.h"
#include "BattleActionGame/BattleGameplayTags.h"
#include "BattleActionGame/Character/BattleCharacterBase.h"
#include "BattleActionGame/Combat/AttackCollisionMethod.h"
#include "BattleActionGame/Combat/BattleCombatManagerComponent.h"
#include "BattleActionGame/Messages/BattleHitMessage.h"
#include "BattleActionGame/Character/BattleCharacterBase.h"
#include "GameFramework/GameStateBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AnimNotifyState_AttackWindow)

UAnimNotifyState_AttackWindow::UAnimNotifyState_AttackWindow()
{
	
}

void UAnimNotifyState_AttackWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	CachedOwnerCharacter = Cast<ABattleCharacterBase>(MeshComp->GetOwner());

	if (CachedOwnerCharacter)
	{
		CachedCombatManager = Cast<UBattleCombatManagerComponent>(CachedOwnerCharacter->GetComponentByClass(UBattleCombatManagerComponent::StaticClass()));
		
		if (CachedOwnerCharacter->IsLocallyControlled())
		{

			TArray<FHitResult> HitResultsWithModify;
			
			for (UAttackCollisionData* CollisionData : CollisionDatas)
			{
				TArray<FHitResult> HitResults;
				CachedCombatManager->GetCollisionMethod(CollisionData->CollisionMethodType)->StartCollisionCheck(HitResults, CollisionData);

				if (HitResults.Num() >0)
				{
					for (const FHitResult& HitResult : HitResults)
					{
						auto Pred = [&HitResult](const FHitResult& OtherHitResult)
						{
							return OtherHitResult.HitObjectHandle == HitResult.HitObjectHandle;
						};

						if (!HitResultsWithModify.ContainsByPredicate(Pred))
						{
							HitResultsWithModify.Add(HitResult);
						}
					}
				}
			}

			
			if (HitResultsWithModify.Num() >0)
			{
				FBattleHitMessage HitMessage;

				HitMessage.HitResults = HitResultsWithModify;
				HitMessage.WindowIndex = AttackIdx;
				HitMessage.HitTime = MeshComp->GetWorld()->GetGameState()->GetServerWorldTimeSeconds();

				UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(MeshComp->GetWorld());
				MessageSubsystem.BroadcastMessage(FBattleGameplayTags::Get().Combat_Attack_Hit, HitMessage);
			}
		}
	}
	
	
	
}

void UAnimNotifyState_AttackWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
}


void UAnimNotifyState_AttackWindow::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (CachedOwnerCharacter && CachedOwnerCharacter->IsLocallyControlled())
	{
		
		TArray<FHitResult> HitResultsWithModify;
		
		for (UAttackCollisionData* CollisionData : CollisionDatas)
		{
			if (CachedCombatManager->GetCollisionMethod(CollisionData->CollisionMethodType)->IsNeedTick())
			{
				TArray<FHitResult> HitResults;
				CachedCombatManager->GetCollisionMethod(CollisionData->CollisionMethodType)->TickCollisionCheck(HitResults, CollisionData, FrameDeltaTime);
				
				if (HitResults.Num() >0)
				{
					for (const FHitResult& HitResult : HitResults)
					{
						auto Pred = [&HitResult](const FHitResult& OtherHitResult)
						{
							return OtherHitResult.HitObjectHandle == HitResult.HitObjectHandle;
						};

						if (!HitResultsWithModify.ContainsByPredicate(Pred))
						{
							HitResultsWithModify.Add(HitResult);
						}
					}
				}
			}
		}
		
		if (HitResultsWithModify.Num() > 0)
		{
			FBattleHitMessage HitMessage;
			HitMessage.HitResults = HitResultsWithModify;
			HitMessage.WindowIndex = AttackIdx;

			UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(MeshComp->GetWorld());
			MessageSubsystem.BroadcastMessage(FBattleGameplayTags::Get().Combat_Attack_Hit, HitMessage);
		}
	}
}

