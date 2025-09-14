#include "AttackCollisionMethod_SocketBasedLineTrace.h"

#include "BattleAbilityTask_HitCheck.h"
#include "BattleCombatData.h"
#include "BattleGameplayAbility_Attack_Parent.h"
#include "BattleActionGame/BattleLogChannels.h"
#include "GameFramework/Character.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AttackCollisionMethod_SocketBasedLineTrace)


void UAttackCollisionMethod_SocketBasedLineTrace::StartCollisionCheck(TArray<FHitResult>& OutHitResult,
	UAttackCollisionData* AttackCollisionData)
{
	UAttackCollisionData_SocketBasedLineTrace* SocketCollisionData = Cast<UAttackCollisionData_SocketBasedLineTrace>(AttackCollisionData);
	StartName = SocketCollisionData->StartSocketName;
	EndName = SocketCollisionData->EndSocketName;
	AttackRadius = SocketCollisionData->AttackRadius;
	CollisionChannel = SocketCollisionData->CollisionChannel;

	Params.bReturnPhysicalMaterial = true;

	MeshComp = Character->GetMesh();

	PreviousStart = MeshComp->GetSocketLocation(*StartName);
	PreviousEnd = MeshComp->GetSocketLocation(*EndName);
	PreviousMiddle = FMath::Lerp(PreviousStart, PreviousEnd, 0.5f);
}

void UAttackCollisionMethod_SocketBasedLineTrace::EndCollisionCheck()
{

}


bool UAttackCollisionMethod_SocketBasedLineTrace::IsNeedTick()
{
	return true;
}

void UAttackCollisionMethod_SocketBasedLineTrace::TickCollisionCheck(TArray<FHitResult>& OutHitResult,
	UAttackCollisionData* AttackCollisionData, float FrameDeltaTime)
{
	BA_DEFAULT_LOG(LogBattle, Log, TEXT("FrameDeltaTime: %f"), FrameDeltaTime);
	
	FVector Start = MeshComp->GetSocketLocation(*StartName);
	FVector End = MeshComp->GetSocketLocation(*EndName);

	if (FrameDeltaTime < DeltaTimeThreshold)
	{
		PerformHitCheck(OutHitResult, Start, End);
	}
	else
	{
		PerformHitCheckWithLerpNotStep(OutHitResult, Start, End);
	}
	
	PreviousStart = Start;
	PreviousEnd = End;
	
}

void UAttackCollisionMethod_SocketBasedLineTrace::PerformHitCheckWithLerpNotStep(TArray<FHitResult>& OutHitResults, FVector& CurWeaponStart,
	FVector& CurWeaponEnd)
{
	BA_DEFAULT_LOG(LogBattle, Log, TEXT("CurS: %s CurE: %s"), *CurWeaponStart.ToString(), *CurWeaponEnd.ToString());
	BA_DEFAULT_LOG(LogBattle, Log, TEXT("PreS: %s PreE: %s"), *PreviousStart.ToString(), *PreviousEnd.ToString());
	
	FVector CurWeaponMiddle = FMath::Lerp(CurWeaponStart, CurWeaponEnd, 0.5f); 
	PreviousMiddle = FMath::Lerp(PreviousStart, PreviousEnd, 0.5f);

	GetWorld()->SweepMultiByChannel(OutHitResults, CurWeaponStart, CurWeaponEnd, FQuat::Identity, CollisionChannel, FCollisionShape::MakeSphere(AttackRadius),Params);

	for (FHitResult& HitResult : OutHitResults)
	{
		if (!CurrentHitResults.Contains(HitResult.GetActor()))
		{
			CurrentHitResults.Add({HitResult.GetActor(), HitResult});
		}
	}

	DrawDebug(CurWeaponStart, CurWeaponEnd, true);

	GetWorld()->SweepMultiByChannel(OutHitResults, CurWeaponStart, PreviousStart, FQuat::Identity, CollisionChannel, FCollisionShape::MakeSphere(AttackRadius),Params);

	for (FHitResult& HitResult : OutHitResults)
	{
		if (!CurrentHitResults.Contains(HitResult.GetActor()))
		{
			CurrentHitResults.Add({HitResult.GetActor(), HitResult});
		}
	}
	
	GetWorld()->SweepMultiByChannel(OutHitResults, CurWeaponEnd, PreviousEnd, FQuat::Identity, CollisionChannel, FCollisionShape::MakeSphere(AttackRadius),Params);

	for (FHitResult& HitResult : OutHitResults)
	{
		if (!CurrentHitResults.Contains(HitResult.GetActor()))
		{
			CurrentHitResults.Add({HitResult.GetActor(), HitResult});
		}
	}
	
	GetWorld()->SweepMultiByChannel(OutHitResults, CurWeaponMiddle, PreviousMiddle, FQuat::Identity, CollisionChannel, FCollisionShape::MakeSphere(AttackRadius),Params);
	
	for (FHitResult& HitResult : OutHitResults)
    {
    	if (!CurrentHitResults.Contains(HitResult.GetActor()))
    	{
    		CurrentHitResults.Add({HitResult.GetActor(), HitResult});
    	}
    }

	OutHitResults.Empty();

	for (TTuple<AActor*, FHitResult>& Item : CurrentHitResults)
	{
		OutHitResults.Add(Item.Value);
	}
	

	DrawDebug(CurWeaponStart, PreviousStart, false);
	DrawDebug(CurWeaponEnd, PreviousEnd, false);
	DrawDebug(CurWeaponMiddle, PreviousMiddle, false);
}

void UAttackCollisionMethod_SocketBasedLineTrace::PerformHitCheck(TArray<FHitResult>& OutHitResults, FVector& CurWeaponStart, FVector& CurWeaponEnd)
{
	GetWorld()->SweepMultiByChannel(OutHitResults, CurWeaponStart, CurWeaponEnd, FQuat::Identity, CollisionChannel, FCollisionShape::MakeSphere(AttackRadius),Params);

	DrawDebug(CurWeaponStart, CurWeaponEnd, true);
}

void UAttackCollisionMethod_SocketBasedLineTrace::DrawDebug(FVector& CurWeaponStart, FVector& CurWeaponEnd, bool bIsHit)
{
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
	DrawDebugCapsule(GetWorld(), CapsuleCenter, CapsuleHalfHeight, AttackRadius,CapsuleRotation, HitColor,false, 10.f);
}
