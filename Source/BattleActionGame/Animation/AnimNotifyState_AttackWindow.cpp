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
	
	TMap<int, FAttackCollisionDebugSpec> CollisionDebugSpecArr;

	if (OwnerCharacter)
	{
		UBattleCombatManagerComponent* CombatManager = Cast<UBattleCombatManagerComponent>(OwnerCharacter->GetComponentByClass(UBattleCombatManagerComponent::StaticClass()));
		CombatManager->OnAttackStart();
		if (OwnerCharacter->IsLocallyControlled() || Cast<AAIController>(OwnerCharacter->GetController()) != nullptr)
		{

			TArray<FHitResult> HitResultsWithModify;
			int Idx = 0;
			for (UAttackCollisionData* CollisionData : CollisionDatas)
			{
				TArray<FHitResult> HitResults;
				FAttackCollisionDebugSpec CollisionDebugSpec;
				
				CombatManager->GetCollisionMethod(CollisionData->CollisionMethodType)->StartCollisionCheck(HitResults, CollisionData, CollisionDebugSpec);

				MakeUniqueHitResults(HitResults, HitResultsWithModify);
				
				CollisionDebugSpecArr.Add(Idx, CollisionDebugSpec);
				Idx++;
			}
			
			SendHitResults(HitResultsWithModify, CombatManager->GetWorld()->GetGameState()->GetServerWorldTimeSeconds(), CombatManager);
			
		}
	}
	
#if 1
		DrawDebugBegin(MeshComp, CollisionDebugSpecArr);
#endif
	
	
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
	
	TMap<int, FAttackCollisionDebugSpec> CollisionDebugSpecArr;
	
	if (ABattleCharacterBase* OwnerCharacter = Cast<ABattleCharacterBase>(MeshComp->GetOwner()))
	{
		if (UBattleCombatManagerComponent* CombatManager = Cast<UBattleCombatManagerComponent>(OwnerCharacter->GetComponentByClass(UBattleCombatManagerComponent::StaticClass())))
		{
			if (OwnerCharacter->IsLocallyControlled() || Cast<AAIController>(OwnerCharacter->GetController()) != nullptr)
			{
				TArray<FHitResult> HitResultsWithModify;
				int Idx = 0;
				
				for (UAttackCollisionData* CollisionData : CollisionDatas)
				{
					FAttackCollisionDebugSpec CollisionDebugSpec;
					if (CombatManager->GetCollisionMethod(CollisionData->CollisionMethodType)->IsNeedTick())
					{
						TArray<FHitResult> HitResults;
						CombatManager->GetCollisionMethod(CollisionData->CollisionMethodType)->TickCollisionCheck(HitResults, CollisionData, FrameDeltaTime, CollisionDebugSpec);
				
						MakeUniqueHitResults(HitResults, HitResultsWithModify);
					}
					CollisionDebugSpecArr.Add(Idx, CollisionDebugSpec);
					Idx++;
				}
				SendHitResults(HitResultsWithModify, CombatManager->GetWorld()->GetGameState()->GetServerWorldTimeSeconds(), CombatManager);
			}
		}
	}
	
#if 0
	DrawDebugTick(MeshComp, CollisionDebugSpecArr);
#endif
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

void UAnimNotifyState_AttackWindow::DrawDebugBegin(USkeletalMeshComponent* MeshComp,
	TMap<int, FAttackCollisionDebugSpec>& CollisionDebugSpecMap)
{
	if (MeshComp->GetWorld() != nullptr)
	{
		if (ACharacter* OwnerCharacter = Cast<ACharacter>(MeshComp->GetOwner()))
		{
			if (!OwnerCharacter->IsLocallyControlled())
			{
				return;
			}
		}
		
	}
	
	int Idx = 0;
	for (UAttackCollisionData* CollisionData : CollisionDatas)
	{
		if (CollisionData->CollisionMethodType == ECollisionMethodType::DirectionalSweep)
		{
			FAttackCollisionDebugSpec CollisionDebugSpec;
			if (CollisionDebugSpecMap.Find(Idx))
			{
				CollisionDebugSpec = CollisionDebugSpecMap[Idx];
			}
			UAttackCollisionMethod_DirectionalSweep::DrawDebugWithStart(MeshComp, CollisionData, CollisionDebugSpec);
		}
		
		Idx++;
	}
}

void UAnimNotifyState_AttackWindow::DrawDebugTick(USkeletalMeshComponent* MeshComp,
	TMap<int, FAttackCollisionDebugSpec>& CollisionDebugSpecMap)
{
	if (MeshComp->GetWorld() != nullptr)
	{
		if (MeshComp->GetNetMode() == ENetMode::NM_DedicatedServer || MeshComp->GetNetMode() == ENetMode::NM_ListenServer)
		{
			return;
		}
	}
	
	int Idx = 0;
	
	for (UAttackCollisionData* CollisionData : CollisionDatas)
	{
		if (CollisionData->CollisionMethodType == ECollisionMethodType::SocketBasedLineTrace)
		{
			FAttackCollisionDebugSpec CollisionDebugSpec;
			if (CollisionDebugSpecMap.Find(Idx))
			{
				CollisionDebugSpec = CollisionDebugSpecMap[Idx];
			}
			
			UAttackCollisionMethod_SocketBasedLineTrace::DrawDebugWithTick(MeshComp, CollisionData, CollisionDebugSpec);
		}
		Idx++;
	}
}


