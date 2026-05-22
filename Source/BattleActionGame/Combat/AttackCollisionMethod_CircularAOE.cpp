#include "AttackCollisionMethod_CircularAOE.h"

#include "BattleCombatData.h"
#include "BattleCombatManagerComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/GameStateBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AttackCollisionMethod_CircularAOE)


PRAGMA_DISABLE_OPTIMIZATION

void UAttackCollisionMethod_CircularAOE::StartCollisionCheck(TArray<FHitResult>& OutHitResult,
	UAttackCollisionData* AttackCollisionData)
{
	if (UAttackCollisionData_CircularAOE* CollisionData = Cast<UAttackCollisionData_CircularAOE>(AttackCollisionData))
	{
		TArray<FVector> AreaCenterData;
		if (UBattleCombatManagerComponent* CombatManagerComponent = Cast<UBattleCombatManagerComponent>(Character->GetComponentByClass(UBattleCombatManagerComponent::StaticClass())))
		{
			AreaCenterData = CombatManagerComponent->GetAreaCenterData();
		}
		
		for (FVector& AttackAreaDataItem : AreaCenterData)
		{
			TArray<FHitResult> HitResults;
			GetWorld()->SweepMultiByChannel(HitResults, AttackAreaDataItem,AttackAreaDataItem, FQuat::Identity, CollisionData->CollisionChannel, FCollisionShape::MakeSphere(CollisionData->AttackRadius), FCollisionQueryParams());

			OutHitResult.Append(HitResults);
			
#if 1
			FVector Start = AttackAreaDataItem;
			FVector End = AttackAreaDataItem;
			float Radius = CollisionData->AttackRadius;
	
			FVector TraceVector = End - Start;
			FVector CapsuleCenter = (Start + End) * 0.5f;  // 캡슐의 중심은 Start와 End의 중간 지점
			float CapsuleHalfHeight = TraceVector.Size() * 0.5f;  // 캡슐의 절반 길이
			FQuat CapsuleRotation = FQuat::FindBetweenNormals(FVector::UpVector, TraceVector.GetSafeNormal());
			DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 3.f, 0, 0);
			DrawDebugCapsule(GetWorld(), CapsuleCenter, CapsuleHalfHeight, Radius,CapsuleRotation, FColor::Green,false, 3.f);
#endif
			
		}	
	}
}

void UAttackCollisionMethod_CircularAOE::EndCollisionCheck()
{
	
}

PRAGMA_ENABLE_OPTIMIZATION
