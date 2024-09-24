#include "BattleAbilityTask_HitCheck.h"

#include "AbilitySystemComponent.h"
#include "BattleActionGame/Physics/BattleCollisionChannels.h"
#include "GameFramework/Character.h"

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

void UBattleAbilityTask_HitCheck::Activate()
{
	Super::Activate();
	
}

void UBattleAbilityTask_HitCheck::OnDestroy(bool bInOwnerFinished)
{
	Super::OnDestroy(bInOwnerFinished);
}

void UBattleAbilityTask_HitCheck::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);
	
	ACharacter* Character = CastChecked<ACharacter>(AbilitySystemComponent->GetAvatarActor());

	USkeletalMeshComponent* MeshComp = Character->GetMesh();

	FVector Start = MeshComp->GetSocketLocation(TEXT("weapon"));
	FVector End = MeshComp->GetSocketLocation(TEXT("weapon_end"));

	TArray<FHitResult> HitResults;

	FCollisionQueryParams Temp;
	
	//GetWorld()->LineTraceMultiByChannel(HitResults, Start, End, Battle_TraceChannel_Weapon, Temp);

	float Radius = 10.f;
	
	GetWorld()->SweepMultiByChannel(HitResults, Start, End, FQuat::Identity, Battle_TraceChannel_Weapon, FCollisionShape::MakeSphere(Radius),Temp);

	FColor HitColor = FColor::Red; 
	
	for (FHitResult HitResult : HitResults)
	{
		UE_LOG(LogTemp, Log, TEXT("Hit %s"),*HitResult.Component->GetName());
		HitColor = FColor::Green;
		if (HitResult.GetComponent()->GetCollisionObjectType() == ECollisionCha)
	}


#if 1
	
	FVector TraceVector = End - Start;
	FVector CapsuleCenter = (Start + End) * 0.5f;  // 캡슐의 중심은 Start와 End의 중간 지점
	float CapsuleHalfHeight = TraceVector.Size() * 0.5f;  // 캡슐의 절반 길이
	FQuat CapsuleRotation = FQuat::FindBetweenNormals(FVector::UpVector, TraceVector.GetSafeNormal());
//	DrawDebugLine(GetWorld(), Start, End, HitColor, false, 10.f, 0, 0);
	DrawDebugCapsule(GetWorld(), CapsuleCenter, CapsuleHalfHeight, Radius,CapsuleRotation, HitColor,false, 10.f);
#endif
}
