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

	ABattleCharacterBase* OwnerCharacter = Cast<ABattleCharacterBase>(MeshComp->GetOwner());

	if (OwnerCharacter)
	{
		UBattleCombatManagerComponent* CombatManager = Cast<UBattleCombatManagerComponent>(OwnerCharacter->GetComponentByClass(UBattleCombatManagerComponent::StaticClass()));
		CombatManager->OnAttackStart();
		if (OwnerCharacter->IsLocallyControlled() || Cast<AAIController>(OwnerCharacter->GetController()) != nullptr)
		{

			TArray<FHitResult> HitResultsWithModify;
			
			for (UAttackCollisionData* CollisionData : CollisionDatas)
			{
				TArray<FHitResult> HitResults;
				CombatManager->GetCollisionMethod(CollisionData->CollisionMethodType)->StartCollisionCheck(HitResults, CollisionData);

				MakeUniqueHitResults(HitResults, HitResultsWithModify);
			}
			
			SendHitResults(HitResultsWithModify, CombatManager->GetWorld()->GetGameState()->GetServerWorldTimeSeconds(), CombatManager);
			
		}
	}
	else
	{
		DrawDebugBegin(MeshComp);
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
	
	if (ABattleCharacterBase* OwnerCharacter = Cast<ABattleCharacterBase>(MeshComp->GetOwner()))
	{
		if (UBattleCombatManagerComponent* CombatManager = Cast<UBattleCombatManagerComponent>(OwnerCharacter->GetComponentByClass(UBattleCombatManagerComponent::StaticClass())))
		{
			if (OwnerCharacter->IsLocallyControlled() || Cast<AAIController>(OwnerCharacter->GetController()) != nullptr)
			{
				TArray<FHitResult> HitResultsWithModify;
		
				for (UAttackCollisionData* CollisionData : CollisionDatas)
				{
					if (CombatManager->GetCollisionMethod(CollisionData->CollisionMethodType)->IsNeedTick())
					{
						TArray<FHitResult> HitResults;
						CombatManager->GetCollisionMethod(CollisionData->CollisionMethodType)->TickCollisionCheck(HitResults, CollisionData, FrameDeltaTime);
				
						MakeUniqueHitResults(HitResults, HitResultsWithModify);
					}
				}
				SendHitResults(HitResultsWithModify, CombatManager->GetWorld()->GetGameState()->GetServerWorldTimeSeconds(), CombatManager);
			}
		}
	}
	else
	{
		DrawDebugTick(MeshComp);
	}
}


void UAnimNotifyState_AttackWindow::MakeUniqueHitResults(const TArray<FHitResult>& InHitResults,
	TArray<FHitResult>& OutHitResult)
{
	if (InHitResults.Num() >0)
	{
		for (const FHitResult& HitResult : InHitResults)
		{
			auto Pred = [&HitResult](const FHitResult& OtherHitResult)
			{
				return OtherHitResult.HitObjectHandle == HitResult.HitObjectHandle;
			};

			if (!OutHitResult.ContainsByPredicate(Pred))
			{
				OutHitResult.Add(HitResult);
			}
		}
	}
}

void UAnimNotifyState_AttackWindow::SendHitResults(const TArray<FHitResult>& SendHitResults, float HitTime,
	UBattleCombatManagerComponent* CombatManager)
{
	if (SendHitResults.Num() >0)
	{
		FBattleHitMessage HitMessage;

		HitMessage.HitResults = SendHitResults;
		HitMessage.WindowIndex = AttackIdx;
		HitMessage.HitTime = HitTime;

		CombatManager->OnHitEvent(HitMessage);
	}
}

void UAnimNotifyState_AttackWindow::DrawDebugBegin(USkeletalMeshComponent* MeshComp)
{
	for (UAttackCollisionData* CollisionData : CollisionDatas)
	{
		if (CollisionData->CollisionMethodType == ECollisionMethodType::DirectionalSweep)
		{
			UAttackCollisionMethod_DirectionalSweep::DrawDebugWithStart(MeshComp, CollisionData);
		}
	}
}

void UAnimNotifyState_AttackWindow::DrawDebugTick(USkeletalMeshComponent* MeshComp)
{
	for (UAttackCollisionData* CollisionData : CollisionDatas)
	{
		if (CollisionData->CollisionMethodType == ECollisionMethodType::SocketBasedLineTrace)
		{
			UAttackCollisionMethod_SocketBasedLineTrace::DrawDebugWithTick(MeshComp, CollisionData);
		}
	}
}

