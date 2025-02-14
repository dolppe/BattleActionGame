#include "AttackCollisionMethod_CircularAOE.h"
#include UE_INLINE_GENERATED_CPP_BY_NAME(AttackCollisionMethod_CircularAOE)




void UAttackCollisionMethod_CircularAOE::StartCollisionCheck()
{
	Super::StartCollisionCheck();
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

void UAttackCollisionMethod_CircularAOE::SendHitResults(const FHitResult& HitResult, const float HitTime)
{
	Super::SendHitResults(HitResult, HitTime);
}


TArray<FVector> UAttackCollisionMethod_CircularAOE::GetBestSpots()
{
	TArray<FVector> Locations;

	TArray<FHitResult> EnemyLocation;

	//GetWorld()->SweepMultiByChannel()
	
	return Locations;
	
}