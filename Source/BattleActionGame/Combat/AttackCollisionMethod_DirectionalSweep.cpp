#include "AttackCollisionMethod_DirectionalSweep.h"

#include "BattleActionGame/BattleLogChannels.h"
#include "BattleCombatData.h"
#include "BattleGameplayAbility_Attack_Parent.h"
#include "BattleActionGame/BattleLogChannels.h"
#include "BattleActionGame/Character/BattleCharacterBase.h"
#include "GameFramework/GameStateBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AttackCollisionMethod_DirectionalSweep)




void UAttackCollisionMethod_DirectionalSweep::StartCollisionCheck(TArray<FHitResult>& OutHitResult,
	UAttackCollisionData* AttackCollisionData, FAttackCollisionDebugSpec& OutCollisionDebugSpec)
{
	if (UAttackCollisionData_DirectionalSweep* CollisionData = Cast<UAttackCollisionData_DirectionalSweep>(AttackCollisionData))
	{
		if (ABattleCharacterBase* CharacterBase = Cast<ABattleCharacterBase>(Character))
		{
			USkeletalMeshComponent* SkeletalMesh = CharacterBase->GetMesh();

			const FRotator Rotation = CharacterBase->GetActorRotation();
			const FRotator NewRotation(Rotation.Pitch + CollisionData->AttackPitchOffset,Rotation.Yaw+CollisionData->AttackYawOffset,0);

			FCollisionQueryParams Temp;
			//const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			const FVector ForwardDirection = NewRotation.Vector();

			FVector Start = SkeletalMesh->GetSocketLocation(*CollisionData->StartSocketName);
			FVector End = Start + ForwardDirection*CollisionData->AttackRange;
	
			float Radius = CollisionData->AttackRadius;

			GetWorld()->SweepMultiByChannel(OutHitResult, Start, End, FQuat::Identity, CollisionData->CollisionChannel, FCollisionShape::MakeSphere(Radius),Temp);
			
			OutCollisionDebugSpec.StartArr.Add(Start);
			OutCollisionDebugSpec.EndArr.Add(End);
			
		}
	}
}

void UAttackCollisionMethod_DirectionalSweep::EndCollisionCheck()
{

}

void UAttackCollisionMethod_DirectionalSweep::DrawDebugWithStart(USkeletalMeshComponent* MeshComp,
	UAttackCollisionData* AttackCollisionData, const FAttackCollisionDebugSpec& CollisionDebugSpec)
{
	if (UAttackCollisionData_DirectionalSweep* CollisionData = Cast<UAttackCollisionData_DirectionalSweep>(AttackCollisionData))
	{
		FVector Start;
		FVector End;
		if (CollisionDebugSpec.IsValid())
		{
			Start = CollisionDebugSpec.StartArr.Last();
			End = CollisionDebugSpec.EndArr.Last();
		}
		else
		{
			FRotator BaseRotation;
			if (ACharacter* OwnerCharacter = Cast<ACharacter>(MeshComp->GetOwner()))
			{
				FRotator Rotation;
				Rotation = OwnerCharacter->GetActorRotation();
				BaseRotation = FRotator(0.0f,Rotation.Yaw,0.0f);
				
			}
			else if (MeshComp != nullptr)
			{
				FRotator Rotation;
				Rotation = MeshComp->GetComponentRotation();
				BaseRotation = FRotator(0.0f,Rotation.Yaw+90.f,0.0f);
			}
			else
			{
				return;
			}
			
			
			const FRotator NewRotation(BaseRotation.Pitch + CollisionData->AttackPitchOffset,BaseRotation.Yaw+CollisionData->AttackYawOffset,0);
		
			Start = MeshComp->GetSocketLocation(*CollisionData->StartSocketName);
			End = Start + NewRotation.Vector()*CollisionData->AttackRange;
		}
		
		FVector TraceVector = End - Start;
		FVector CapsuleCenter = (Start + End) * 0.5f;  // 캡슐의 중심은 Start와 End의 중간 지점
		float CapsuleHalfHeight = TraceVector.Size() * 0.5f + CollisionData->AttackRadius;
		FQuat CapsuleRotation = FQuat::FindBetweenNormals(FVector::UpVector, TraceVector.GetSafeNormal());

		UWorld* World = MeshComp->GetWorld();
		
		FVector UpVector = MeshComp->GetUpVector().GetSafeNormal();
		
		DrawDebugLine(World, Start, End, FColor::Green, false, 2.f, 0, 0);
		DrawDebugCapsule(World, CapsuleCenter, CapsuleHalfHeight, CollisionData->AttackRadius,CapsuleRotation, FColor::Emerald,false, 5.f);
	}

}

