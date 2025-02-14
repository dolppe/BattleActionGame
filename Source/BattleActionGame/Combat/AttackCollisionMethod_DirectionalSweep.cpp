#include "AttackCollisionMethod_DirectionalSweep.h"

#include "BattleCombatData.h"
#include "BattleGameplayAbility_Attack_Parent.h"
#include "BattleActionGame/Character/BattleCharacterBase.h"
#include "GameFramework/GameStateBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AttackCollisionMethod_DirectionalSweep)

PRAGMA_DISABLE_OPTIMIZATION
void UAttackCollisionMethod_DirectionalSweep::StartCollisionCheck()
{
	if (UAttackCollisionData_DirectionalSweep* CollisionData = Cast<UAttackCollisionData_DirectionalSweep>(AttackCollisionData))
	{
		TArray<FHitResult> HitResults;

		ABattleCharacterBase* CharacterBase = Cast<ABattleCharacterBase>(OriginGameplayAbility->GetAvatarActorFromActorInfo());
		USkeletalMeshComponent* SkeletalMesh = CharacterBase->GetMesh();

		const FRotator Rotation = CharacterBase->GetActorRotation();
		const FRotator YawRotation(0,Rotation.Yaw+CollisionData->AttackRotationOffset,0);

		FCollisionQueryParams Temp;
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		FVector Start = SkeletalMesh->GetSocketLocation(*CollisionData->StartSocketName);
		FVector End = Start + ForwardDirection*CollisionData->AttackRange;
	
		float Radius = CollisionData->AttackRadius;

		GetWorld()->SweepMultiByChannel(HitResults, Start, End, FQuat::Identity, CollisionData->CollisionChannel, FCollisionShape::MakeSphere(Radius),Temp);

		for (FHitResult HitResult : HitResults)
		{
			SendHitResults(HitResult, GetWorld()->GetGameState()->GetServerWorldTimeSeconds());
		}

		UWorld* World = GetWorld();

#if 1

		FVector TraceVector = End - Start;
		FVector CapsuleCenter = (Start + End) * 0.5f;  // 캡슐의 중심은 Start와 End의 중간 지점
		float CapsuleHalfHeight = TraceVector.Size() * 0.5f;  // 캡슐의 절반 길이
		FQuat CapsuleRotation = FQuat::FindBetweenNormals(FVector::UpVector, TraceVector.GetSafeNormal());
		DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 10.f, 0, 0);
		DrawDebugCapsule(GetWorld(), CapsuleCenter, CapsuleHalfHeight, Radius,CapsuleRotation, FColor::Green,false, 10.f);
	
#endif
		
	}


}

PRAGMA_ENABLE_OPTIMIZATION

void UAttackCollisionMethod_DirectionalSweep::EndCollisionCheck()
{
	Super::EndCollisionCheck();
}

void UAttackCollisionMethod_DirectionalSweep::SetCollisionData(UAttackCollisionData* InAttackCollisionData,
	UBattleGameplayAbility_Attack_Parent* InGameplayAbility)
{
	Super::SetCollisionData(InAttackCollisionData, InGameplayAbility);
}

void UAttackCollisionMethod_DirectionalSweep::SendHitResults(const FHitResult& HitResult, const float HitTime)
{
	Super::SendHitResults(HitResult, HitTime);
}
