#include "BattleUtilityAction_Movement.h"

#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "BattleActionGame/Character/BattleCharacterBase.h"
#include "BattleActionGame/Enemy/BattleEnemyCharacter.h"
#include "BattleActionGame/AbilitySystem/Abilities/BattleGameplayAbility.h"
#include "BattleActionGame/Combat/BattleCombatManagerComponent.h"
#include "BattleActionGame/Enemy/UtilityAI/BattleConsiderationFactors.h"
#include "BattleActionGame/Enemy/UtilityAI/BattleUtilityAxis.h"
#include "BattleActionGame/Environment/BattleWorldInfoSubsystem.h"
#include "BattleActionGame/Navigation/FindPathFunction.h"
#include "BattleActionGame/Navigation/WorldRiskGridMap.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleUtilityAction_Movement)

PRAGMA_DISABLE_OPTIMIZATION

UBattleUtilityAction_Wait::UBattleUtilityAction_Wait()
{
}

void UBattleUtilityAction_Wait::StartAction()
{
	Super::StartAction();
	
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
	{
		this->bIsCompletedAction = true;
	},
	WaitSeconds, false);
	
}

void UBattleUtilityAction_Wait::TickAction(float DeltaTime)
{
	Super::TickAction(DeltaTime);
}

void UBattleUtilityAction_Wait::EndAction()
{
	Super::EndAction();
}

UBAttleUtilityAction_PlayMontage::UBAttleUtilityAction_PlayMontage()
{
}

void UBAttleUtilityAction_PlayMontage::StartAction()
{
	Super::StartAction();
	
	UConsiderationFactors* ConsiderList = ConsiderationFactors.Get();
	if (!IsValid(ConsiderList))
	{
		bIsCompletedAction = true;
		return;
	}
	
	ABattleCharacterBase* MyCharacter = ConsiderList->GetMyCharacter();
	if (MyCharacter)
	{
		bIsCompletedAction = true;
		return;
	}
	
	MyCharacter->NetPlayMontage(AnimMontage);
	AnimInstance = MyCharacter->GetMesh()->GetAnimInstance();
	
}

void UBAttleUtilityAction_PlayMontage::TickAction(float DeltaTime)
{
	if (AnimInstance->Montage_IsPlaying(AnimMontage))
	{
		bIsCompletedAction = false;
	}
	else
	{
		bIsCompletedAction = true;
	}
}

void UBAttleUtilityAction_PlayMontage::EndAction()
{
	Super::EndAction();
}

UBattleUtilityAction_BackAway::UBattleUtilityAction_BackAway()
{
}

void UBattleUtilityAction_BackAway::StartAction()
{
	Super::StartAction();
	
	UConsiderationFactors* ConsiderList = ConsiderationFactors.Get();
	if (!IsValid(ConsiderList))
	{
		bIsCompletedAction = true;
		return;
	}
	
	MyCharacter = ConsiderList->GetMyCharacter();
	SelectedTarget = ConsiderList->GetSelectedTarget();
	
	if (!IsValid(MyCharacter) || !IsValid(SelectedTarget))
	{
		bIsCompletedAction = true;
		return;
	}
	
	MyCharacter->bUseControllerRotationYaw = false;
	FVector ToTarget = (SelectedTarget->GetActorLocation() - MyCharacter->GetActorLocation()).GetSafeNormal2D();
	WantedLocation = MyCharacter->GetActorLocation() -(ToTarget * BackAwayDistance);
	
	DrawDebugPoint(GetWorld(), WantedLocation, 10.f, FColor::Green, false, 5.f);
	
	if (UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld()))
	{
		FNavLocation ProjectedNavLocation;
		
		if (NavSystem->ProjectPointToNavigation(WantedLocation, ProjectedNavLocation, FVector(700.f,700.f,1000.f)))
		{
			WantedLocation = ProjectedNavLocation.Location;
			DrawDebugPoint(GetWorld(), WantedLocation, 10.f, FColor::Red, false, 5.f);
		}
	}
	
	AIController = Cast<AAIController>(MyCharacter->GetController());
	if (AIController)
	{
		RequestResult = AIController->MoveToLocation(WantedLocation);
	}
}

void UBattleUtilityAction_BackAway::TickAction(float DeltaTime)
{
	if (RequestResult != EPathFollowingRequestResult::RequestSuccessful)
	{
		bIsCompletedAction = true;
		return;
	}
	if (SelectedTarget == nullptr)
	{
		bIsCompletedAction = true;
		return;
	}
	
	
	FVector TargetLocation = SelectedTarget->GetActorLocation();
	FVector MyLocation = MyCharacter->GetActorLocation();
	FRotator CurrentRotation = MyCharacter->GetActorRotation();

	FVector DirectionToTarget = (TargetLocation - MyLocation).GetSafeNormal();
	FRotator TargetRotation = DirectionToTarget.Rotation();

	float RotationDifference = FMath::Abs(CurrentRotation.Yaw - TargetRotation.Yaw);

	const float Tolerance = 1.0f;

	if (RotationDifference > Tolerance)
	{
		FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, 8.f);

		NewRotation.Pitch = 0;

		// 캐릭터의 회전을 업데이트
		MyCharacter->SetActorRotation(NewRotation);	
	}
	
	if (AIController)
	{
		if (UPathFollowingComponent* PathFollowingComponent = AIController->GetPathFollowingComponent())
		{
			if (EPathFollowingStatus::Type::Moving == PathFollowingComponent->GetStatus())
			{
				return;
			}
		}
	}
	
	bIsCompletedAction = true;
}

void UBattleUtilityAction_BackAway::EndAction()
{
	MyCharacter->bUseControllerRotationYaw = true;
	Super::EndAction();
}

UBattleUtilityAction_Strafe::UBattleUtilityAction_Strafe()
{
}

void UBattleUtilityAction_Strafe::StartAction()
{
	Super::StartAction();
	
	UConsiderationFactors* ConsiderList = ConsiderationFactors.Get();
	if (!IsValid(ConsiderList))
	{
		bIsCompletedAction = true;
		return;
	}
	
	MyCharacter = ConsiderList->GetMyCharacter();
	ABattleCharacterBase* SelectedTarget = ConsiderList->GetSelectedTarget();
	
	if (!IsValid(MyCharacter) || !IsValid(SelectedTarget))
	{
		bIsCompletedAction = true;
		return;
	}
	
	MyCharacter->bUseControllerRotationYaw = false;
	
	RequestResult = EPathFollowingRequestResult::Failed;

	int RightCount = 0;
	int LeftCount = 0;
	
	FVector MyRightVector = MyCharacter->GetActorRightVector();
	FVector MyLocation = MyCharacter->GetActorLocation();
	
	for (int32 Idx = 0; Idx < ConsiderList->GetCandidateArrNum(EAxisType::Target); ++Idx)
	{
		AActor* TargetActor = ConsiderList->GetCandidatePtr(EAxisType::Target, Idx);
		
		if (!IsValid(TargetActor))
		{
			continue;
		}
		FVector ToTargetVector = TargetActor->GetActorLocation() - MyLocation;
		if (MyRightVector.Dot(ToTargetVector) >= 0.0f)
		{
			RightCount++;
		}
		else
		{
			LeftCount++;
		}
	}
	
	if (RightCount >= LeftCount)
	{
		bIsRight = true;
	}
	else
	{
		bIsRight = false;
	}
	
	FVector SelectedTargetLocation = SelectedTarget->GetActorLocation();
	float Radius = FVector::Dist2D(MyLocation, SelectedTargetLocation);
	FVector Dir = (MyLocation - SelectedTargetLocation).GetSafeNormal2D();
	
	float AngleDeg = bIsRight ? 30.f : -30.f;
	FVector RotateVector = Dir.RotateAngleAxis(AngleDeg, FVector::UpVector);
	
	WantedLocation = SelectedTargetLocation + RotateVector * Radius;
	
	FVector MoveVector = WantedLocation - MyLocation;
	if (MoveVector.Length() > MaxDistance)
	{
		WantedLocation = MyLocation + MoveVector.GetSafeNormal2D() * MaxDistance;
	}
	
	if (UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld()))
	{
		FNavLocation ProjectedNavLocation;
		
		if (NavSystem->ProjectPointToNavigation(WantedLocation, ProjectedNavLocation, FVector(700.f,700.f,1000.f)))
		{
			WantedLocation = ProjectedNavLocation.Location;
		}
	}
	
	
	AIController = Cast<AAIController>(MyCharacter->GetController());
	if (AIController)
	{
		RequestResult = AIController->MoveToLocation(WantedLocation);
	}
}

void UBattleUtilityAction_Strafe::TickAction(float DeltaTime)
{
	if (RequestResult != EPathFollowingRequestResult::RequestSuccessful)
	{
		bIsCompletedAction = true;
		return;
	}
	
	
	UConsiderationFactors* ConsiderList = ConsiderationFactors.Get();
	if (!IsValid(ConsiderList))
	{
		bIsCompletedAction = true;
		return;
	}
	
	ABattleCharacterBase* SelectedTarget = ConsiderList->GetSelectedTarget();
	
	if (!IsValid(MyCharacter) || !IsValid(SelectedTarget))
	{
		bIsCompletedAction = true;
		return;
	}
	
	FVector ToTargetLocation = (SelectedTarget->GetActorLocation() - MyCharacter->GetActorLocation()).GetSafeNormal2D();
	
	FRotator TargetRotation = ToTargetLocation.Rotation();
	FRotator CurrentRotation = MyCharacter->GetActorRotation();
	float InterpSpeed = (MyCharacter->GetActorForwardVector().GetSafeNormal2D().Dot(ToTargetLocation) <=0.5f) ?15.f : 8.f;
	
	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, InterpSpeed);

	NewRotation.Pitch = 0;

	// 캐릭터의 회전을 업데이트
	MyCharacter->SetActorRotation(NewRotation);
	
	if (AIController)
	{
		if (UPathFollowingComponent* PathFollowingComponent = AIController->GetPathFollowingComponent())
		{
			if (EPathFollowingStatus::Type::Moving == PathFollowingComponent->GetStatus())
			{
				return;
			}
		}
	}
	
	bIsCompletedAction = true;
}

void UBattleUtilityAction_Strafe::EndAction()
{
	if (MyCharacter)
	{
		MyCharacter->bUseControllerRotationYaw = true;
	}
	
	Super::EndAction();
}

UBattleUtilityAction_AbilityAction::UBattleUtilityAction_AbilityAction()
{
}

void UBattleUtilityAction_AbilityAction::StartAction()
{
	Super::StartAction();
	
	UConsiderationFactors* ConsiderList = ConsiderationFactors.Get();
	if (!IsValid(ConsiderList))
	{
		bIsCompletedAction = true;
		return;
	}
	
	ABattleCharacterBase* MyCharacter = ConsiderList->GetMyCharacter();
	
	if (!IsValid(MyCharacter))
	{
		bIsCompletedAction = true;
		return;
	}

	ASC = MyCharacter->GetAbilitySystemComponent();

	if (ASC)
	{
		AbilitySpec = ASC->FindAbilitySpecFromClass(AbilityAction);
		if (AbilitySpec)
		{
			ASC->TryActivateAbility(AbilitySpec->Handle);
		}
	}
	
}

void UBattleUtilityAction_AbilityAction::TickAction(float DeltaTime)
{

	if (AbilitySpec->IsActive())
	{
		return;
	}
	bIsCompletedAction = true;
}

void UBattleUtilityAction_AbilityAction::EndAction()
{
	if (AbilitySpec->IsActive())
	{
		ASC->CancelAbility(AbilitySpec->Ability);
	}
}

void UBattleUtilityAction_AbilityAction::OnEndAbility()
{
}

UBattleUtilityAction_MoveToTarget::UBattleUtilityAction_MoveToTarget()
{
}

void UBattleUtilityAction_MoveToTarget::StartAction()
{
	Super::StartAction();
	
	UConsiderationFactors* ConsiderList = ConsiderationFactors.Get();
	if (!IsValid(ConsiderList))
	{
		bIsCompletedAction = true;
		return;
	}
	
	MyCharacter = ConsiderList->GetMyCharacter();
	
	if (!IsValid(MyCharacter))
	{
		bIsCompletedAction = true;
		return;
	}
}

void UBattleUtilityAction_MoveToTarget::EndAction()
{

	
	Super::EndAction();
}

void UBattleUtilityAction_MoveToTarget::TickAction(float DeltaTime)
{
	UConsiderationFactors* ConsiderList = ConsiderationFactors.Get();
	if (!IsValid(ConsiderList))
	{
		bIsCompletedAction = true;
		return;
	}
	
	SelectedTarget = ConsiderList->GetSelectedTarget();
	
	if (!IsValid(SelectedTarget))
	{
		bIsCompletedAction = true;
		return;
	}
	

	FVector MyLocation = MyCharacter->GetActorLocation();
	FVector TargetLocation = SelectedTarget->GetActorLocation();
	
	if (FVector::DistSquared(TargetLocation,MyLocation) <= WantedDistance * WantedDistance)
	{
		bIsCompletedAction = true;
	}
	
	FVector DirectionAwayFromTarget = (TargetLocation - MyLocation).GetSafeNormal();
	FRotator TargetRotation = DirectionAwayFromTarget.Rotation();

	TargetLocation.Z = 0.0f;
	TargetRotation.Pitch = 0.0f;
	TargetRotation.Roll = 0.0f;

	// 현재 회전값에서 목표 회전값으로 부드럽게 회전ssd
	FRotator NewRotation = FMath::RInterpTo(MyCharacter->GetActorRotation(), TargetRotation, DeltaTime, 10.0f);

	// 캐릭터에 새 회전값 적용
	MyCharacter->SetActorRotation(NewRotation);
	MyCharacter->AddMovementInput(DirectionAwayFromTarget);


}

UBattleUtilityAction_TurnToTargetAndStare::UBattleUtilityAction_TurnToTargetAndStare()
{
}

void UBattleUtilityAction_TurnToTargetAndStare::StartAction()
{
	Super::StartAction();
	
	UConsiderationFactors* ConsiderList = ConsiderationFactors.Get();
	if (!IsValid(ConsiderList))
	{
		bIsCompletedAction = true;
		return;
	}
	
	MyCharacter = ConsiderList->GetMyCharacter();
	SelectedTarget = ConsiderList->GetSelectedTarget();
	
	
	if (!IsValid(MyCharacter) || !IsValid(SelectedTarget))
	{
		bIsCompletedAction = true;
		return;
	}
	
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
	
	bIsEnded = false;
	
	GetWorld()->GetTimerManager().SetTimer(TimerHandle,[this]()
	{
		this->bIsEnded = true;
	}, 2.f, false);
}

void UBattleUtilityAction_TurnToTargetAndStare::EndAction()
{
	Super::EndAction();
}

void UBattleUtilityAction_TurnToTargetAndStare::TickAction(float DeltaTime)
{
	if (bIsEnded)
	{
		bIsCompletedAction = true;
	}
	
	FVector TargetLocation = SelectedTarget->GetActorLocation();
	FVector MyLocation = MyCharacter->GetActorLocation();
	FRotator CurrentRotation = MyCharacter->GetActorRotation();

	FVector DirectionToTarget = (TargetLocation - MyLocation).GetSafeNormal();
	FRotator TargetRotation = DirectionToTarget.Rotation();

	float RotationDifference = FMath::Abs(CurrentRotation.Yaw - TargetRotation.Yaw);

	const float Tolerance = 1.0f;

	if (RotationDifference > Tolerance)
	{
		FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, 8.f);

		NewRotation.Pitch = 0;

		// 캐릭터의 회전을 업데이트
		MyCharacter->SetActorRotation(NewRotation);	
	}
}

UBattleUtilityAction_PatrolAround::UBattleUtilityAction_PatrolAround()
{
}

void UBattleUtilityAction_PatrolAround::StartAction()
{
	Super::StartAction();
	
	UConsiderationFactors* ConsiderList = ConsiderationFactors.Get();
	if (!IsValid(ConsiderList))
	{
		bIsCompletedAction = true;
		return;
	}
	
	ABattleCharacterBase* MyCharacter = ConsiderList->GetMyCharacter();
	
	if (!IsValid(MyCharacter))
	{
		bIsCompletedAction = true;
		return;
	}

	RequestResult = EPathFollowingRequestResult::Type::Failed;
	
	float PatrolRadius = 10000.0f;

	FVector CharacterLocation = MyCharacter->GetActorLocation();

	// Navigation System을 이용해 랜덤 지점을 찾음
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (NavSystem)
	{
		// 랜덤 지점 생성
		FVector RandomLocation;
		FNavLocation NavLocation;
		if (NavSystem->GetRandomPointInNavigableRadius(CharacterLocation, PatrolRadius, NavLocation))
		{
			RandomLocation = NavLocation.Location;
			//RandomLocation = FVector(3000,3000,0);

			AIController = Cast<AAIController>(MyCharacter->GetController());
			if (AIController)
			{
				RequestResult = AIController->MoveToLocation(RandomLocation);
			}
		}
	}
	
}

PRAGMA_ENABLE_OPTIMIZATION

void UBattleUtilityAction_PatrolAround::EndAction()
{
	if (AIController->IsFollowingAPath())
	{
		AIController->StopMovement();
	}

	Super::EndAction();
}

void UBattleUtilityAction_PatrolAround::TickAction(float DeltaTime)
{
	if (RequestResult != EPathFollowingRequestResult::Type::RequestSuccessful)
	{
		bIsCompletedAction = true;
	}

	if (AIController)
	{
		if (UPathFollowingComponent* PathFollowingComponent = AIController->GetPathFollowingComponent())
		{
			if (EPathFollowingStatus::Type::Moving == PathFollowingComponent->GetStatus())
			{
				return;
			}
		}
	}
	
	bIsCompletedAction = true;
	
}


UBattleUtilityAction_MoveToLocation::UBattleUtilityAction_MoveToLocation()
{
}

PRAGMA_DISABLE_OPTIMIZATION

void UBattleUtilityAction_MoveToLocation::StartAction()
{
	Super::StartAction();
	
	UConsiderationFactors* ConsiderList = ConsiderationFactors.Get();
	if (!IsValid(ConsiderList))
	{
		bIsCompletedAction = true;
		return;
	}
	
	ABattleCharacterBase* MyCharacter = ConsiderList->GetMyCharacter();
	
	if (!IsValid(MyCharacter))
	{
		bIsCompletedAction = true;
		return;
	}
	
	RequestResult = EPathFollowingRequestResult::Type::Failed;
	
	AIController = Cast<AAIController>(MyCharacter->GetController());
	
	FVector TargetLocation = GetLocation();

	if (TargetLocation.ContainsNaN())
	{
		EndAction();
	}
	else
	{
		RequestResult = AIController->MoveToLocation(TargetLocation);
	}
}

void UBattleUtilityAction_MoveToLocation::EndAction()
{
	if (AIController->IsFollowingAPath())
	{
		AIController->StopMovement();
	}
	Super::EndAction();
}

void UBattleUtilityAction_MoveToLocation::TickAction(float DeltaTime)
{
	if (RequestResult != EPathFollowingRequestResult::Type::RequestSuccessful)
	{
		bIsCompletedAction = true;
	}

	if (AIController)
	{
		if (UPathFollowingComponent* PathFollowingComponent = AIController->GetPathFollowingComponent())
		{
			if (EPathFollowingStatus::Type::Moving == PathFollowingComponent->GetStatus())
			{
				const FNavPathSharedPtr Path = PathFollowingComponent ? PathFollowingComponent->GetPath() : FNavPathSharedPtr();
				if (Path.IsValid())
				{
					const TArray<FNavPathPoint>& Points = Path->GetPathPoints();
					const FVector HeightOffset(0.0f, 0.0f, 30.0f);

					for (int32 Index = 1; Index < Points.Num(); ++Index)
					{
						DrawDebugLine(
							GetWorld(),
							Points[Index - 1].Location + HeightOffset,
							Points[Index].Location + HeightOffset,
							FColor::Green,
							false,
							30.0f,
							0,
							10.0f);
					}
				}
					
					
				return;
			}
		}
	}
	bIsCompletedAction = true;
}

PRAGMA_ENABLE_OPTIMIZATION

FVector UBattleUtilityAction_MoveToLocation::GetLocation()
{
	return FVector(NAN,NAN,NAN);
}

UBattleUtilityAction_MoveToSelectedSpot::UBattleUtilityAction_MoveToSelectedSpot()
{
}

FVector UBattleUtilityAction_MoveToSelectedSpot::GetLocation()
{
	return SelectedSpot;
}

UBattleUtilityAction_MoveToBestSpot::UBattleUtilityAction_MoveToBestSpot()
{
}

FVector UBattleUtilityAction_MoveToBestSpot::GetLocation()
{
	UConsiderationFactors* ConsiderList = ConsiderationFactors.Get();
	
	if (!IsValid(ConsiderList))
	{
		bIsCompletedAction = true;
		return FVector();
	}
	
	return ConsiderList->GetBestSpotLocation();
}

UBattleUtilityAction_MoveToBestSpotWithRoar::UBattleUtilityAction_MoveToBestSpotWithRoar()
{
}

void UBattleUtilityAction_MoveToBestSpotWithRoar::StartAction()
{
	UConsiderationFactors* ConsiderList = ConsiderationFactors.Get();
	
	if (!IsValid(ConsiderList))
	{
		bIsCompletedAction = true;
		return;
	}
	
	ABattleCharacterBase* MyCharacter = ConsiderList->GetMyCharacter();
	
	if (!IsValid(MyCharacter))
	{
		bIsCompletedAction = true;
		return;
	}
	
	UAbilitySystemComponent* ASC = MyCharacter->GetAbilitySystemComponent();

	if (ASC)
	{
		ASC->TryActivateAbilityByClass(RoarGA);
	}
	
	Super::StartAction();
}

UBattleUtilityAction_MoveToBestSpotWithRushAttack::UBattleUtilityAction_MoveToBestSpotWithRushAttack()
{
}

void UBattleUtilityAction_MoveToBestSpotWithRushAttack::StartAction()
{
	Super::StartAction();
	
	UConsiderationFactors* ConsiderList = ConsiderationFactors.Get();
	
	if (!IsValid(ConsiderList))
	{
		bIsCompletedAction = true;
		return;
	}
	
	ABattleCharacterBase* MyCharacter = ConsiderList->GetMyCharacter();
	
	if (!IsValid(MyCharacter))
	{
		bIsCompletedAction = true;
		return;
	}

	bAbilityCompleted = false;
	
	UAbilitySystemComponent* ASC = MyCharacter->GetAbilitySystemComponent();

	FVector BestSpotLocation = ConsiderList->GetBestSpotLocation();

	AActor* SelectedTargetActor = nullptr;
	float TargetMinDistance = BIG_NUMBER; 

	for (int32 Idx = 0; Idx < ConsiderList->GetCandidateArrNum(EAxisType::Target); ++Idx)
	{
		AActor* TargetActor = ConsiderList->GetCandidatePtr(EAxisType::Target, Idx);
		if (!IsValid(TargetActor))
		{
			continue;
		}
		float Distance = (TargetActor->GetActorLocation() - BestSpotLocation).SizeSquared();
		if (Distance < TargetMinDistance)
		{
			TargetMinDistance = Distance;
			SelectedTargetActor = TargetActor;
		}
	}
	
	if (UBattleCombatManagerComponent* CombatManager =Cast<UBattleCombatManagerComponent>(MyCharacter->GetComponentByClass(UBattleCombatManagerComponent::StaticClass())))
	{
		CombatManager->SetCurrentTargetActor(SelectedTargetActor);
	}
	
	if (ASC)
	{
		AbilitySpec = ASC->FindAbilitySpecFromClass(RushAttackGA);
		ASC->TryActivateAbility(AbilitySpec->Handle);
	}
	
	
}

void UBattleUtilityAction_MoveToBestSpotWithRushAttack::TickAction(float DeltaTime)
{
	if (AbilitySpec->IsActive())
	{
		return;
	}
	else if (!bAbilityCompleted)
	{
		UConsiderationFactors* ConsiderList = ConsiderationFactors.Get();
	
		if (!IsValid(ConsiderList))
		{
			bIsCompletedAction = true;
			return;
		}
	
		ABattleCharacterBase* MyCharacter = ConsiderList->GetMyCharacter();
	
		if (!IsValid(MyCharacter))
		{
			bIsCompletedAction = true;
			return;
		}
		RequestResult = EPathFollowingRequestResult::Type::Failed;

		ABattleEnemyCharacter* EnemyCharacter = Cast<ABattleEnemyCharacter>(MyCharacter);
		AIController = Cast<AAIController>(EnemyCharacter->GetController());
	
		FVector TargetLocation = ConsiderList->GetBestSpotLocation();

		if (TargetLocation.ContainsNaN())
		{
			EndAction();
			bIsCompletedAction = true;
		}
		else
		{
			RequestResult = AIController->MoveToLocation(TargetLocation);
		}
		bAbilityCompleted = true;
		return;
	}


	if (RequestResult != EPathFollowingRequestResult::Type::RequestSuccessful)
	{
		bIsCompletedAction = true;
	}
	else if (AIController)
	{
		if (UPathFollowingComponent* PathFollowingComponent = AIController->GetPathFollowingComponent())
		{
			if (EPathFollowingStatus::Type::Moving == PathFollowingComponent->GetStatus())
			{
				return;
			}
		}
	}
	bIsCompletedAction = true;
}

UBattleUtilityAction_MoveToLocationWithGridMap::UBattleUtilityAction_MoveToLocationWithGridMap()
{
}

void UBattleUtilityAction_MoveToLocationWithGridMap::StartAction()
{
	Super::StartAction();
	
	UConsiderationFactors* ConsiderList = ConsiderationFactors.Get();
	
	if (!IsValid(ConsiderList))
	{
		bIsCompletedAction = true;
		return;
	}
	
	ABattleCharacterBase* MyCharacter = ConsiderList->GetMyCharacter();
	
	if (!IsValid(MyCharacter))
	{
		bIsCompletedAction = true;
		return;
	}
	
	RequestResult = EPathFollowingRequestResult::Type::Failed;
	
	AIController = Cast<AAIController>(MyCharacter->GetController());
	
	FVector TargetLocation = ConsiderList->GetBestSpotLocation();

	if (TargetLocation.ContainsNaN())
	{
		EndAction();
	}
	else
	{
		RequestResult = AIController->MoveToLocation(TargetLocation);
	}
}

void UBattleUtilityAction_MoveToLocationWithGridMap::EndAction()
{
	if (AIController->IsFollowingAPath())
	{
		AIController->StopMovement();
	}
	Super::EndAction();
}

void UBattleUtilityAction_MoveToLocationWithGridMap::TickAction(float DeltaTime)
{
	if (RequestResult != EPathFollowingRequestResult::Type::RequestSuccessful)
	{
		bIsCompletedAction = true;
	}

	if (AIController)
	{
		if (UPathFollowingComponent* PathFollowingComponent = AIController->GetPathFollowingComponent())
		{
			if (EPathFollowingStatus::Type::Moving == PathFollowingComponent->GetStatus())
			{
				return;
			}
		}
	}
	bIsCompletedAction = true;
}

UBattleUtilityAction_MoveToRuin::UBattleUtilityAction_MoveToRuin()
{
}

FVector UBattleUtilityAction_MoveToRuin::GetLocation()
{
	UBattleWorldInfoSubsystem* WorldInfoSubsystem = GetWorld()->GetSubsystem<UBattleWorldInfoSubsystem>();
	return WorldInfoSubsystem->GetUtilitySpot(EUtilitySpotType::ChargePoison)->GetActorLocation();
}


UBattleUtilityAction_MoveToDesert::UBattleUtilityAction_MoveToDesert()
{
}

FVector UBattleUtilityAction_MoveToDesert::GetLocation()
{
	UBattleWorldInfoSubsystem* WorldInfoSubsystem = GetWorld()->GetSubsystem<UBattleWorldInfoSubsystem>();
	return WorldInfoSubsystem->GetUtilitySpot(EUtilitySpotType::ChargeElectricity)->GetActorLocation();
}

UBattleUtilityAction_MoveToCave::UBattleUtilityAction_MoveToCave()
{
}

FVector UBattleUtilityAction_MoveToCave::GetLocation()
{
	UBattleWorldInfoSubsystem* WorldInfoSubsystem = GetWorld()->GetSubsystem<UBattleWorldInfoSubsystem>();
	return WorldInfoSubsystem->GetUtilitySpot(EUtilitySpotType::Resting)->GetActorLocation();
}

PRAGMA_ENABLE_OPTIMIZATION


