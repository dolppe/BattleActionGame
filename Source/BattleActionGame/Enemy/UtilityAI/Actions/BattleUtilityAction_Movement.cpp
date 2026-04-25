#include "BattleUtilityAction_Movement.h"

#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "BattleActionGame/Character/BattleCharacterBase.h"
#include "BattleActionGame/Enemy/BattleEnemyCharacter.h"
#include "BattleActionGame/AbilitySystem/Abilities/BattleGameplayAbility.h"
#include "BattleActionGame/Combat/BattleCombatManagerComponent.h"
#include "BattleActionGame/Environment/BattleWorldInfoSubsystem.h"
#include "BattleActionGame/Navigation/FindPathFunction.h"
#include "BattleActionGame/Navigation/WorldRiskGridMap.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleUtilityAction_Movement)

UBAttleUtilityAction_PlayMontage::UBAttleUtilityAction_PlayMontage()
{
	Priority = 2;
}

void UBAttleUtilityAction_PlayMontage::StartAction()
{
	Super::StartAction();
	
	CachedAIComponent->ConsiderList->MyCharacter->NetPlayMontage(AnimMontage);
	AnimInstance = CachedAIComponent->ConsiderList->MyCharacter->GetMesh()->GetAnimInstance();
}

bool UBAttleUtilityAction_PlayMontage::TickAction(float DeltaTime)
{
	if (AnimInstance->Montage_IsPlaying(AnimMontage))
	{
		return false;
	}
	return true;
}

void UBAttleUtilityAction_PlayMontage::EndAction()
{
	Super::EndAction();
}

UBattleUtilityAction_BackAway::UBattleUtilityAction_BackAway()
{
	Priority = 2;
}

void UBattleUtilityAction_BackAway::StartAction()
{
	Super::StartAction();
	
	MyCharacter = CachedAIComponent->ConsiderList->MyCharacter;
	SelectedTarget = CachedAIComponent->ConsiderList->SelectedTarget;
	CachedAIComponent->ConsiderList->MyCharacter->bUseControllerRotationYaw = false;
	
	if (SelectedTarget != nullptr)
	{
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
		
		
		AIController = Cast<AAIController>(CachedAIComponent->ConsiderList->MyCharacter->GetController());
		if (AIController)
		{
			RequestResult = AIController->MoveToLocation(WantedLocation);
		}
		
	}
	
	
	
}

bool UBattleUtilityAction_BackAway::TickAction(float DeltaTime)
{
	if (RequestResult != EPathFollowingRequestResult::RequestSuccessful)
	{
		return true;
	}
	if (SelectedTarget == nullptr)
	{
		return true;
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
				return false;
			}
		}
	}
	
	return true;	
}

void UBattleUtilityAction_BackAway::EndAction()
{
	CachedAIComponent->ConsiderList->MyCharacter->bUseControllerRotationYaw = true;
	Super::EndAction();
}

UBattleUtilityAction_Strafe::UBattleUtilityAction_Strafe()
{
	Priority = 2;
}

void UBattleUtilityAction_Strafe::StartAction()
{
	Super::StartAction();
	
	RequestResult = EPathFollowingRequestResult::Failed;
	if (CachedAIComponent)
	{
		if (!CachedAIComponent->ConsiderList->SelectedTarget)
		{
			return;
		}
		CachedAIComponent->ConsiderList->MyCharacter->bUseControllerRotationYaw = false;
		
		int RightCount = 0;
		int LeftCount = 0;
		
		FVector MyRightVector = CachedAIComponent->ConsiderList->MyCharacter->GetActorRightVector();
		FVector MyLocation = CachedAIComponent->ConsiderList->MyCharacter->GetActorLocation();
		
		for (AActor* TargetActor : CachedAIComponent->ConsiderList->TargetActors)
		{
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
		
		FVector SelectedTargetLocation = CachedAIComponent->ConsiderList->SelectedTarget->GetActorLocation();
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
		
		
		AIController = Cast<AAIController>(CachedAIComponent->ConsiderList->MyCharacter->GetController());
		if (AIController)
		{
			RequestResult = AIController->MoveToLocation(WantedLocation);
		}
		
	}
	

	
	
}

bool UBattleUtilityAction_Strafe::TickAction(float DeltaTime)
{
	if (RequestResult != EPathFollowingRequestResult::RequestSuccessful)
	{
		return true;
	}
	
	AActor* MyCharacter =  CachedAIComponent->ConsiderList->MyCharacter;
	AActor* SelectedTarget = CachedAIComponent->ConsiderList->SelectedTarget;
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
				return false;
			}
		}
	}
	
	return true;	
}

void UBattleUtilityAction_Strafe::EndAction()
{
	CachedAIComponent->ConsiderList->MyCharacter->bUseControllerRotationYaw = true;
	
	Super::EndAction();
}

UBattleUtilityAction_AbilityAction::UBattleUtilityAction_AbilityAction()
{
	Priority = 2;
}

void UBattleUtilityAction_AbilityAction::StartAction()
{
	Super::StartAction();

	ASC = CachedAIComponent->ConsiderList->MyCharacter->GetAbilitySystemComponent();

	if (ASC)
	{
		AbilitySpec = ASC->FindAbilitySpecFromClass(AbilityAction);
		if (AbilitySpec)
		{
			ASC->TryActivateAbility(AbilitySpec->Handle);
		}
	}
	
}

bool UBattleUtilityAction_AbilityAction::TickAction(float DeltaTime)
{

	if (AbilitySpec->IsActive())
	{
		return false;
	}
	else
	{
		return true;
	}
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
	
	MyCharacter = CachedAIComponent->ConsiderList->MyCharacter;
}

void UBattleUtilityAction_MoveToTarget::EndAction()
{

	
	Super::EndAction();
}

bool UBattleUtilityAction_MoveToTarget::TickAction(float DeltaTime)
{
	if (CachedAIComponent->ConsiderList->SelectedTarget)
	{
		FVector MyLocation = MyCharacter->GetActorLocation();
		FVector TargetLocation = CachedAIComponent->ConsiderList->SelectedTarget->GetActorLocation();
		
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

		return false;
	}
	else
	{
		return true;
	}
}

UBattleUtilityAction_TurnToTargetAndStare::UBattleUtilityAction_TurnToTargetAndStare()
{
}

void UBattleUtilityAction_TurnToTargetAndStare::StartAction()
{
	Super::StartAction();

	SelectedTarget = CachedAIComponent->ConsiderList->SelectedTarget;
	MyCharacter = CachedAIComponent->ConsiderList->MyCharacter;
	
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

bool UBattleUtilityAction_TurnToTargetAndStare::TickAction(float DeltaTime)
{
	if (bIsEnded)
	{
		return true;
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
	
	return false;	
}

UBattleUtilityAction_PatrolAround::UBattleUtilityAction_PatrolAround()
{
	Priority = 1;
}

PRAGMA_DISABLE_OPTIMIZATION

void UBattleUtilityAction_PatrolAround::StartAction()
{
	Super::StartAction();

	RequestResult = EPathFollowingRequestResult::Type::Failed;
	
	float PatrolRadius = 10000.0f;
	ABattleCharacterBase* MyCharacter = CachedAIComponent->ConsiderList->MyCharacter;

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

bool UBattleUtilityAction_PatrolAround::TickAction(float DeltaTime)
{
	if (RequestResult != EPathFollowingRequestResult::Type::RequestSuccessful)
	{
		return true;
	}

	if (AIController)
	{
		if (UPathFollowingComponent* PathFollowingComponent = AIController->GetPathFollowingComponent())
		{
			if (EPathFollowingStatus::Type::Moving == PathFollowingComponent->GetStatus())
			{
				return false;
			}
		}
	}
	
	return true;
	
}


UBattleUtilityAction_MoveToLocation::UBattleUtilityAction_MoveToLocation()
{
}

void UBattleUtilityAction_MoveToLocation::StartAction()
{
	Super::StartAction();
	
	RequestResult = EPathFollowingRequestResult::Type::Failed;

	ABattleEnemyCharacter* EnemyCharacter = Cast<ABattleEnemyCharacter>(CachedAIComponent->ConsiderList->MyCharacter);
	AIController = Cast<AAIController>(EnemyCharacter->GetController());
	
	FVector TargetLocation = GetLocation();

	if (TargetLocation.ContainsNaN())
	{
		EndAction();
	}
	else
	{
		RequestResult = AIController->MoveToLocation(TargetLocation);
		//RequestResult = AIController->MoveToLocation(TargetLocation, -1, true, true, false, true, UNavQueryFilter_CustomCost::StaticClass());
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

bool UBattleUtilityAction_MoveToLocation::TickAction(float DeltaTime)
{
	if (RequestResult != EPathFollowingRequestResult::Type::RequestSuccessful)
	{
		return true;
	}

	if (AIController)
	{
		if (UPathFollowingComponent* PathFollowingComponent = AIController->GetPathFollowingComponent())
		{
			if (EPathFollowingStatus::Type::Moving == PathFollowingComponent->GetStatus())
			{
				return false;
			}
		}
	}
	return true;
}

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
	Priority = 3;
}

FVector UBattleUtilityAction_MoveToBestSpot::GetLocation()
{
	return CachedAIComponent->ConsiderList->BestSpotLocation;
}

UBattleUtilityAction_MoveToBestSpotWithRoar::UBattleUtilityAction_MoveToBestSpotWithRoar()
{
	Priority = 4;
}

void UBattleUtilityAction_MoveToBestSpotWithRoar::StartAction()
{
	UAbilitySystemComponent* ASC = CachedAIComponent->ConsiderList->MyCharacter->GetAbilitySystemComponent();

	if (ASC)
	{
		ASC->TryActivateAbilityByClass(RoarGA);
	}
	
	Super::StartAction();
}

UBattleUtilityAction_MoveToBestSpotWithRushAttack::UBattleUtilityAction_MoveToBestSpotWithRushAttack()
{
	Priority =4;
}

void UBattleUtilityAction_MoveToBestSpotWithRushAttack::StartAction()
{
	Super::StartAction();

	bAbilityCompleted = false;
	
	UAbilitySystemComponent* ASC = CachedAIComponent->ConsiderList->MyCharacter->GetAbilitySystemComponent();

	FVector TargetLocation = CachedAIComponent->ConsiderList->BestSpotLocation;

	ABattleCharacterBase* SelectedTargetActor = nullptr;
	float MinDistance = BIG_NUMBER; 
	

	for (ABattleCharacterBase* TargetActor : CachedAIComponent->ConsiderList->TargetActors)
	{
		FVector TargetActorLocation = TargetActor->GetActorLocation();

		float Distance = (TargetActorLocation - TargetLocation).SizeSquared();
		if (Distance < MinDistance)
		{
			MinDistance = Distance;
			SelectedTargetActor = TargetActor;
		}
	}

	if (SelectedTargetActor != nullptr)
	{
		if (UBattleCombatManagerComponent* CombatManager =Cast<UBattleCombatManagerComponent>(CachedAIComponent->ConsiderList->MyCharacter->GetComponentByClass(UBattleCombatManagerComponent::StaticClass())))
		{
			CombatManager->SetCurrentTargetActor(SelectedTargetActor);
		}
	}

	if (ASC)
	{
		AbilitySpec = ASC ->FindAbilitySpecFromClass(RushAttackGA);
		ASC->TryActivateAbility(AbilitySpec->Handle);
	}
	
	
}

bool UBattleUtilityAction_MoveToBestSpotWithRushAttack::TickAction(float DeltaTime)
{
	if (AbilitySpec->IsActive())
	{
		return false;
	}
	else if (!bAbilityCompleted)
	{
		RequestResult = EPathFollowingRequestResult::Type::Failed;

		ABattleEnemyCharacter* EnemyCharacter = Cast<ABattleEnemyCharacter>(CachedAIComponent->ConsiderList->MyCharacter);
		AIController = Cast<AAIController>(EnemyCharacter->GetController());
	
		FVector TargetLocation = CachedAIComponent->ConsiderList->BestSpotLocation;

		if (TargetLocation.ContainsNaN())
		{
			EndAction();
			return true;
		}
		else
		{
			RequestResult = AIController->MoveToLocation(TargetLocation);
		}
		bAbilityCompleted = true;
		return false;
	}


	if (RequestResult != EPathFollowingRequestResult::Type::RequestSuccessful)
	{
		return true;	
	}
	else if (AIController)
	{
		if (UPathFollowingComponent* PathFollowingComponent = AIController->GetPathFollowingComponent())
		{
			if (EPathFollowingStatus::Type::Moving == PathFollowingComponent->GetStatus())
			{
				return false;
			}
		}
	}
	return true;
}

UBattleUtilityAction_MoveToLocationWithGridMap::UBattleUtilityAction_MoveToLocationWithGridMap()
{
}

void UBattleUtilityAction_MoveToLocationWithGridMap::StartAction()
{
	Super::StartAction();
	
	RequestResult = EPathFollowingRequestResult::Type::Failed;

	ABattleEnemyCharacter* EnemyCharacter = Cast<ABattleEnemyCharacter>(CachedAIComponent->ConsiderList->MyCharacter);
	AIController = Cast<AAIController>(EnemyCharacter->GetController());
	
	FVector TargetLocation = CachedAIComponent->ConsiderList->BestSpotLocation;

	if (TargetLocation.ContainsNaN())
	{
		EndAction();
	}
	else
	{
		UWorldRiskGridMapSubSystem* GridMapSubSystem = GetWorld()->GetSubsystem<UWorldRiskGridMapSubSystem>();
		GridMapSubSystem->UpdateRisk(CachedAIComponent->ConsiderList->MyCharacter->GetActorLocation());
		
		RequestResult = AIController->MoveToLocation(TargetLocation);
		//RequestResult = AIController->MoveToLocation(TargetLocation, -1, true, true, false, true, UNavQueryFilter_CustomCost::StaticClass());
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

bool UBattleUtilityAction_MoveToLocationWithGridMap::TickAction(float DeltaTime)
{
	if (RequestResult != EPathFollowingRequestResult::Type::RequestSuccessful)
	{
		return true;
	}

	if (AIController)
	{
		if (UPathFollowingComponent* PathFollowingComponent = AIController->GetPathFollowingComponent())
		{
			if (EPathFollowingStatus::Type::Moving == PathFollowingComponent->GetStatus())
			{
				return false;
			}
		}
	}
	return true;
}

UBattleUtilityAction_MoveToRuin::UBattleUtilityAction_MoveToRuin()
{
	Priority = 4;
}

FVector UBattleUtilityAction_MoveToRuin::GetLocation()
{
	UBattleWorldInfoSubsystem* WorldInfoSubsystem = GetWorld()->GetSubsystem<UBattleWorldInfoSubsystem>();
	return WorldInfoSubsystem->GetUtilitySpot(EUtilitySpotType::ChargePoison)->GetActorLocation();
}


UBattleUtilityAction_MoveToDesert::UBattleUtilityAction_MoveToDesert()
{
	Priority = 4;
}

FVector UBattleUtilityAction_MoveToDesert::GetLocation()
{
	UBattleWorldInfoSubsystem* WorldInfoSubsystem = GetWorld()->GetSubsystem<UBattleWorldInfoSubsystem>();
	return WorldInfoSubsystem->GetUtilitySpot(EUtilitySpotType::ChargeElectricity)->GetActorLocation();
}

UBattleUtilityAction_MoveToCave::UBattleUtilityAction_MoveToCave()
{
	Priority = 4;
}

FVector UBattleUtilityAction_MoveToCave::GetLocation()
{
	UBattleWorldInfoSubsystem* WorldInfoSubsystem = GetWorld()->GetSubsystem<UBattleWorldInfoSubsystem>();
	return WorldInfoSubsystem->GetUtilitySpot(EUtilitySpotType::Resting)->GetActorLocation();
}


