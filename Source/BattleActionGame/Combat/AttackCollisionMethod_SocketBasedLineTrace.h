#pragma once

#include "AttackCollisionMethod.h"
#include "AttackCollisionMethod_SocketBasedLineTrace.generated.h"

class UBattleAbilityTask_HitCheck;

UCLASS()
class UAttackCollisionMethod_SocketBasedLineTrace : public UAttackCollisionMethod
{
	GENERATED_BODY()
public:

	virtual void StartCollisionCheck(TArray<FHitResult>& OutHitResult, UAttackCollisionData* AttackCollisionData) override;
	virtual void EndCollisionCheck() override;

	virtual bool IsNeedTick() override;

	virtual void TickCollisionCheck(TArray<FHitResult>& OutHitResult, UAttackCollisionData* AttackCollisionData, float FrameDeltaTime) override;
	static void DrawDebugWithTick(USkeletalMeshComponent* MeshComp, UAttackCollisionData* AttackCollisionData);

protected:
	
	void PerformHitCheckWithLerpNotStep(TArray<FHitResult>& OutHitResults, FVector& CurWeaponStart, FVector& CurWeaponEnd);
	void PerformHitCheck(TArray<FHitResult>& OutHitResults, FVector& CurWeaponStart, FVector& CurWeaponEnd);

	void DrawDebug(FVector& CurWeaponStart, FVector& CurWeaponEnd, bool bIsHit);
	
	UPROPERTY()
	FVector PreviousStart;

	UPROPERTY()
	FVector PreviousMiddle;
	
	UPROPERTY()
	FVector PreviousEnd;

	FString StartName;
	FString EndName;
	float AttackRadius;
	TEnumAsByte<ECollisionChannel> CollisionChannel;
	TObjectPtr<USkeletalMeshComponent> MeshComponent;

	FCollisionQueryParams Params;

	const float DeltaTimeThreshold = 0.05f;

	TMap<AActor*,FHitResult> CurrentHitResults;
	
};

