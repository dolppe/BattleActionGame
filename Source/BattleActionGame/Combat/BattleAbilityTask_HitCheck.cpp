#include "BattleAbilityTask_HitCheck.h"

#include "AbilitySystemComponent.h"
#include "BattleActionGame/Character/BattleCharacterBase.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameStateBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleAbilityTask_HitCheck)

UBattleAbilityTask_HitCheck::UBattleAbilityTask_HitCheck()
{
	bTickingTask = true;
}

UBattleAbilityTask_HitCheck* UBattleAbilityTask_HitCheck::CreateTask(UGameplayAbility* OwningAbility)
{
	UBattleAbilityTask_HitCheck* NewTask = NewAbilityTask<UBattleAbilityTask_HitCheck>(OwningAbility);
	return NewTask;
}

void UBattleAbilityTask_HitCheck::SetHitCheckData(FString InStartName, FString InEndName, float InAttackRadius,
	TEnumAsByte<ECollisionChannel> InCollisionChannel)
{
	StartName = InStartName;
	EndName = InEndName;
	AttackRadius = InAttackRadius;
	CollisionChannel = InCollisionChannel;
}

void UBattleAbilityTask_HitCheck::Activate()
{
	Super::Activate();
	
	ACharacter* Character = CastChecked<ACharacter>(AbilitySystemComponent->GetAvatarActor());
	MeshComp = Character->GetMesh();
	
	Temp.bReturnPhysicalMaterial = true;

	PreviousStart = MeshComp->GetSocketLocation(*StartName);
	PreviousEnd = MeshComp->GetSocketLocation(*EndName);
	PreviousMiddle = FMath::Lerp(PreviousStart, PreviousEnd, 0.5f);
}

void UBattleAbilityTask_HitCheck::OnDestroy(bool bInOwnerFinished)
{
	Super::OnDestroy(bInOwnerFinished);
}

void UBattleAbilityTask_HitCheck::PerformHitCheckWithLerp(FVector& CurWeaponStart, FVector& CurWeaponEnd, float DeltaTime)
{
	for (int32 i = 0; i <= NumSteps; i++)
	{
		// 보간 비율 계산
		float Alpha = i / static_cast<float>(NumSteps);

		// 선형 보간으로 중간 위치 계산
		FVector LerpStart = FMath::Lerp(PreviousStart, CurWeaponStart, Alpha);
		FVector LerpEnd = FMath::Lerp(PreviousEnd, CurWeaponEnd, Alpha);

		TArray<FHitResult> OutHitResultByStep;
		
		// Sweep 처리
		GetWorld()->SweepMultiByChannel(OutHitResultByStep, LerpStart, LerpEnd, FQuat::Identity, CollisionChannel, FCollisionShape::MakeSphere(AttackRadius),Temp);

		bool bIsHit = HandleHit(OutHitResultByStep);

		if (i == NumSteps)
		{
			bIsHit = true;
		}
		else
		{
			bIsHit = false;
		}

		DrawDebug(LerpStart, LerpEnd, bIsHit);
	}
}

void UBattleAbilityTask_HitCheck::PerformHitCheckWithLerpNotStep(FVector& CurWeaponStart, FVector& CurWeaponEnd,
	float DeltaTime)
{
	PerformHitCheck(CurWeaponStart, CurWeaponEnd);

	TArray<FHitResult> OutHitResults;

	FVector CurWeaponMiddle = FMath::Lerp(CurWeaponStart, CurWeaponEnd, 0.5f); 
	PreviousMiddle = FMath::Lerp(PreviousStart, PreviousEnd, 0.5f);

	GetWorld()->SweepMultiByChannel(OutHitResults, CurWeaponStart, PreviousStart, FQuat::Identity, CollisionChannel, FCollisionShape::MakeSphere(AttackRadius),Temp);
	GetWorld()->SweepMultiByChannel(OutHitResults, CurWeaponEnd, PreviousEnd, FQuat::Identity, CollisionChannel, FCollisionShape::MakeSphere(AttackRadius),Temp);
	GetWorld()->SweepMultiByChannel(OutHitResults, CurWeaponMiddle, PreviousMiddle, FQuat::Identity, CollisionChannel, FCollisionShape::MakeSphere(AttackRadius),Temp);
	

	bool bIsHit = HandleHit(OutHitResults);

	DrawDebug(CurWeaponStart, PreviousStart, false);
	DrawDebug(CurWeaponEnd, PreviousEnd, false);
	DrawDebug(CurWeaponMiddle, PreviousMiddle, false);
	
}

void UBattleAbilityTask_HitCheck::PerformHitCheck(FVector& CurWeaponStart, FVector& CurWeaponEnd)
{
	TArray<FHitResult> OutHitResults;
	
	GetWorld()->SweepMultiByChannel(OutHitResults, CurWeaponStart, CurWeaponEnd, FQuat::Identity, CollisionChannel, FCollisionShape::MakeSphere(AttackRadius),Temp);

	bool bIsHit = HandleHit(OutHitResults);

	DrawDebug(CurWeaponStart, CurWeaponEnd, true);
}

bool UBattleAbilityTask_HitCheck::HandleHit(TArray<FHitResult>& HitResults)
{
	bool bIsHit = false;
	for (FHitResult& HitResult : HitResults)
	{
		if (HitResult.GetActor()->IsA(ABattleCharacterBase::StaticClass()))
		{
			OnHitChecked.Broadcast(HitResult, GetWorld()->GetGameState()->GetServerWorldTimeSeconds());
			if (!bIsHit)
			{
				bIsHit = true;
			}
		}
	}

	return bIsHit;
	
}

PRAGMA_DISABLE_OPTIMIZATION

void UBattleAbilityTask_HitCheck::DrawDebug(FVector& CurWeaponStart, FVector& CurWeaponEnd, bool bIsHit)
{
#if 1
	
	FColor HitColor = FColor::Red;

	UWorld* World = GetWorld();
	
	if (bIsHit)
	{
		HitColor = FColor::Green;
	}
	FVector TraceVector = CurWeaponEnd - CurWeaponStart;
	FVector CapsuleCenter = (CurWeaponStart + CurWeaponEnd) * 0.5f;  // 캡슐의 중심은 Start와 End의 중간 지점
	float CapsuleHalfHeight = TraceVector.Size() * 0.5f;  // 캡슐의 절반 길이
	FQuat CapsuleRotation = FQuat::FindBetweenNormals(FVector::UpVector, TraceVector.GetSafeNormal());
	//	DrawDebugLine(GetWorld(), Start, End, HitColor, false, 10.f, 0, 0);
	DrawDebugCapsule(GetWorld(), CapsuleCenter, CapsuleHalfHeight, AttackRadius,CapsuleRotation, HitColor,false, 10.f);
#endif
}

PRAGMA_ENABLE_OPTIMIZATION

void UBattleAbilityTask_HitCheck::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);
	
	FVector Start = MeshComp->GetSocketLocation(*StartName);
	FVector End = MeshComp->GetSocketLocation(*EndName);

	if (DeltaTime < DeltaTimeThreshold)
	{
		PerformHitCheck(Start, End);
	}
	else
	{
		PerformHitCheckWithLerpNotStep(Start, End, DeltaTime);
	}
	
	PreviousStart = Start;
	PreviousEnd = End;


}
