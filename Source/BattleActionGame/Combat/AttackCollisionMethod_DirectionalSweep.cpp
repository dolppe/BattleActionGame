#include "AttackCollisionMethod_DirectionalSweep.h"

#include "BattleCombatData.h"
#include "BattleGameplayAbility_Attack_Parent.h"
#include "BattleActionGame/Character/BattleCharacterBase.h"
#include "GameFramework/GameStateBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AttackCollisionMethod_DirectionalSweep)


void UAttackCollisionMethod_DirectionalSweep::StartCollisionCheck(TArray<FHitResult>& OutHitResult,
	UAttackCollisionData* AttackCollisionData)
{
	if (UAttackCollisionData_DirectionalSweep* CollisionData = Cast<UAttackCollisionData_DirectionalSweep>(AttackCollisionData))
	{
		ABattleCharacterBase* CharacterBase = Cast<ABattleCharacterBase>(Character);
		USkeletalMeshComponent* SkeletalMesh = CharacterBase->GetMesh();

		const FRotator Rotation = CharacterBase->GetActorRotation();
		const FRotator YawRotation(0,Rotation.Yaw+CollisionData->AttackRotationOffset,0);

		FCollisionQueryParams Temp;
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		FVector Start = SkeletalMesh->GetSocketLocation(*CollisionData->StartSocketName);
		FVector End = Start + ForwardDirection*CollisionData->AttackRange;
	
		float Radius = CollisionData->AttackRadius;

		GetWorld()->SweepMultiByChannel(OutHitResult, Start, End, FQuat::Identity, CollisionData->CollisionChannel, FCollisionShape::MakeSphere(Radius),Temp);



#if 1

		FVector TraceVector = End - Start;
		FVector CapsuleCenter = (Start + End) * 0.5f;  // 캡슐의 중심은 Start와 End의 중간 지점
		float CapsuleHalfHeight = TraceVector.Size() * 0.5f;  // 캡슐의 절반 길이
		FQuat CapsuleRotation = FQuat::FindBetweenNormals(FVector::UpVector, TraceVector.GetSafeNormal());
		DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 2.f, 0, 0);
		DrawDebugCapsule(GetWorld(), CapsuleCenter, CapsuleHalfHeight, Radius,CapsuleRotation, FColor::Green,false, 2.f);
	
#endif
		
	}
}

void UAttackCollisionMethod_DirectionalSweep::EndCollisionCheck()
{

}

void UAttackCollisionMethod_DirectionalSweep::DrawDebugWithStart(USkeletalMeshComponent* MeshComp,
	UAttackCollisionData* AttackCollisionData)
{
	if (UAttackCollisionData_DirectionalSweep* CollisionData = Cast<UAttackCollisionData_DirectionalSweep>(AttackCollisionData))
	{
		const FRotator Rotation = MeshComp->GetComponentRotation();
		const FRotator YawRotation(0,Rotation.Yaw+CollisionData->AttackRotationOffset+90,0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		
		FVector Start = MeshComp->GetSocketLocation(*CollisionData->StartSocketName);
		FVector End = Start + ForwardDirection*CollisionData->AttackRange;

		FVector TraceVector = End - Start;
		FVector CapsuleCenter = (Start + End) * 0.5f;  // 캡슐의 중심은 Start와 End의 중간 지점
		float CapsuleHalfHeight = TraceVector.Size() * 0.5f;  // 캡슐의 절반 길이
		FQuat CapsuleRotation = FQuat::FindBetweenNormals(FVector::UpVector, TraceVector.GetSafeNormal());

		UWorld* World = MeshComp->GetWorld();
	
		DrawDebugLine(World, Start, End, FColor::Green, false, 2.f, 0, 0);
		DrawDebugCapsule(World, CapsuleCenter, CapsuleHalfHeight, CollisionData->AttackRadius,CapsuleRotation, FColor::Green,false, 2.f);
	}

}
