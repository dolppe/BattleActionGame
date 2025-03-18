#include "BattleUtilityAction_Movement.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "BattleActionGame/Character/BattleCharacterBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleUtilityAction_Movement)

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

UBattleUtilityAction_TurnToTarget::UBattleUtilityAction_TurnToTarget()
{
}

void UBattleUtilityAction_TurnToTarget::StartAction()
{
	Super::StartAction();

	SelectedTarget = CachedAIComponent->ConsiderList->SelectedTarget;
	MyCharacter = CachedAIComponent->ConsiderList->MyCharacter;
}

void UBattleUtilityAction_TurnToTarget::EndAction()
{
	Super::EndAction();
}

bool UBattleUtilityAction_TurnToTarget::TickAction(float DeltaTime)
{
	FVector TargetLocation = SelectedTarget->GetActorLocation();
	FVector MyLocation = MyCharacter->GetActorLocation();
	FRotator CurrentRotation = MyCharacter->GetActorRotation();

	FVector DirectionToTarget = (TargetLocation - MyLocation).GetSafeNormal();
	FRotator TargetRotation = DirectionToTarget.Rotation();

	float RotationDifference = FMath::Abs(CurrentRotation.Yaw - TargetRotation.Yaw);

	const float Tolerance = 1.0f;

	if (RotationDifference <= Tolerance)
	{
		// 회전 차이가 허용 오차 이내면 더 이상 회전하지 않음
		return true;
	}
	
	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, 12.f);

	NewRotation.Pitch = 0;

	// 캐릭터의 회전을 업데이트
	MyCharacter->SetActorRotation(NewRotation);

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

UBattleUtilityAction_RunCombat::UBattleUtilityAction_RunCombat()
{
}

void UBattleUtilityAction_RunCombat::StartAction()
{
	Super::StartAction();
	
	MyCharacter = CachedAIComponent->ConsiderList->MyCharacter;
}

void UBattleUtilityAction_RunCombat::EndAction()
{
	Super::EndAction();
}

bool UBattleUtilityAction_RunCombat::TickAction(float DeltaTime)
{
	if (CachedAIComponent->ConsiderList->SelectedTarget)
	{
		FVector MyLocation = MyCharacter->GetActorLocation();
		FVector TargetLocation = CachedAIComponent->ConsiderList->SelectedTarget->GetActorLocation();
		
		FVector DirectionAwayFromTarget = (MyLocation - TargetLocation).GetSafeNormal();

		FRotator TargetRotation = DirectionAwayFromTarget.Rotation();

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
