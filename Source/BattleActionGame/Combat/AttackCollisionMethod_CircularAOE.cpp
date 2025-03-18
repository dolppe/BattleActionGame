#include "AttackCollisionMethod_CircularAOE.h"

#include "BattleCombatData.h"
#include "GameFramework/GameStateBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AttackCollisionMethod_CircularAOE)




void UAttackCollisionMethod_CircularAOE::StartCollisionCheck()
{
	if (UAttackCollisionData_CircularAOE* CollisionData = Cast<UAttackCollisionData_CircularAOE>(AttackCollisionData))
	{
		if (AreaCenterData.IsEmpty())
		{
			AreaCenterData = GetBestSpots();
		}

		for (FVector& AttackAreaDataItem : AreaCenterData)
		{
			TArray<FHitResult> HitResults;
			GetWorld()->SweepMultiByChannel(HitResults, AttackAreaDataItem,AttackAreaDataItem, FQuat::Identity, CollisionData->CollisionChannel, FCollisionShape::MakeSphere(CollisionData->AttackRadius), FCollisionQueryParams());

			for (FHitResult HitResult : HitResults)
			{
				SendHitResults(HitResult, GetWorld()->GetGameState()->GetServerWorldTimeSeconds());
			}

#if 1
			FVector Start = AttackAreaDataItem;
			FVector End = AttackAreaDataItem;
			float Radius = CollisionData->AttackRadius;
	
			FVector TraceVector = End - Start;
			FVector CapsuleCenter = (Start + End) * 0.5f;  // 캡슐의 중심은 Start와 End의 중간 지점
			float CapsuleHalfHeight = TraceVector.Size() * 0.5f;  // 캡슐의 절반 길이
			FQuat CapsuleRotation = FQuat::FindBetweenNormals(FVector::UpVector, TraceVector.GetSafeNormal());
			DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 10.f, 0, 0);
			DrawDebugCapsule(GetWorld(), CapsuleCenter, CapsuleHalfHeight, Radius,CapsuleRotation, FColor::Green,false, 10.f);
#endif
			
		}	
	}
	

}

void UAttackCollisionMethod_CircularAOE::EndCollisionCheck()
{
	Super::EndCollisionCheck();
}

void UAttackCollisionMethod_CircularAOE::SetCollisionData(UAttackCollisionData* InAttackCollisionData,
	UBattleGameplayAbility_Attack_Parent* InGameplayAbility)
{
	Super::SetCollisionData(InAttackCollisionData, InGameplayAbility);
}

void UAttackCollisionMethod_CircularAOE::SetAreaCenterData(TArray<FVector> InAreaCenterData)
{
	AreaCenterData = InAreaCenterData;
}

void UAttackCollisionMethod_CircularAOE::SendHitResults(const FHitResult& HitResult, const float HitTime)
{
	Super::SendHitResults(HitResult, HitTime);
}


TArray<FVector> UAttackCollisionMethod_CircularAOE::GetBestSpots()
{
	TArray<FVector> Result;
	TArray<FVector> Locations;

	TArray<FHitResult> EnemyLocation;

	//GetWorld()->SweepMultiByChannel()
	
	return Result;
	
}