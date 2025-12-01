#include "AnimNotifyState_AttackWindow.h"

#include "AIController.h"
#include "GameplayMessageSubsystem.h"
#include "BattleActionGame/BattleGameplayTags.h"
#include "BattleActionGame/Character/BattleCharacterBase.h"
#include "BattleActionGame/Combat/AttackCollisionMethod.h"
#include "BattleActionGame/Combat/BattleCombatManagerComponent.h"
#include "BattleActionGame/Messages/BattleHitMessage.h"
#include "BattleActionGame/Character/BattleCharacterBase.h"
#include "BattleActionGame/Combat/AttackCollisionMethod_DirectionalSweep.h"
#include "BattleActionGame/Combat/AttackCollisionMethod_SocketBasedLineTrace.h"
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
		
		if (CachedOwnerCharacter->IsLocallyControlled() || Cast<AAIController>(CachedOwnerCharacter->GetController()) != nullptr)
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

				CachedCombatManager->OnHitEvent(HitMessage);
			}
		}
	}
	else
	{
		for (UAttackCollisionData* CollisionData : CollisionDatas)
		{
			DrawDebugBegin(MeshComp, CollisionData);
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

	if (CachedOwnerCharacter && CachedCombatManager)
	{
		if (CachedOwnerCharacter->IsLocallyControlled() || Cast<AAIController>(CachedOwnerCharacter->GetController()) != nullptr)
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

				CachedCombatManager->OnHitEvent(HitMessage);
			}
		}
		
	}
	else
	{
		for (UAttackCollisionData* CollisionData : CollisionDatas)
		{
			DrawDebugTick(MeshComp, CollisionData);
		}
	}
}

void UAnimNotifyState_AttackWindow::DrawDebugBegin(USkeletalMeshComponent* MeshComp, UAttackCollisionData* AttackCollisionData)
{
	for (UAttackCollisionData* CollisionData : CollisionDatas)
	{
		if (CollisionData->CollisionMethodType == ECollisionMethodType::DirectionalSweep)
		{
			UAttackCollisionMethod_DirectionalSweep::DrawDebugWithStart(MeshComp, AttackCollisionData);
		}
	}
}

void UAnimNotifyState_AttackWindow::DrawDebugTick(USkeletalMeshComponent* MeshComp, UAttackCollisionData* AttackCollisionData)
{
	for (UAttackCollisionData* CollisionData : CollisionDatas)
	{
		if (CollisionData->CollisionMethodType == ECollisionMethodType::SocketBasedLineTrace)
		{
			UAttackCollisionMethod_SocketBasedLineTrace::DrawDebugWithTick(MeshComp, AttackCollisionData);
		}
	}
}

