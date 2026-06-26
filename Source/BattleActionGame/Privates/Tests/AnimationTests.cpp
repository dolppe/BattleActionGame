
#if WITH_DEV_AUTOMATION_TESTS

#include "AbilitySystemComponent.h"
#include "BattleActorTestFixture.h"
#include "BattleCombatTests.h"
#include "BattleActionGame/BattleGameplayTags.h"
#include "BattleActionGame/Animation/AnimNode_DestroyedParts.h"
#include "BattleActionGame/Animation/AnimNotifyState_AttackWindow.h"
#include "BattleActionGame/Animation/AnimNotify_CommitState.h"
#include "BattleActionGame/Animation/AnimNotify_EndingState.h"
#include "BattleActionGame/Animation/AnimNotify_OpeningState.h"
#include "BattleActionGame/Combat/BattleCombatManagerComponent.h"
#include "Components/CapsuleComponent.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FAnimNode_DestoryPartsTest, "BattleAction.Animation.AnimNode.DestroyParts", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter);
IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(FAnimNotify_AttackStateTest, FBattleActorTestFixture,"BattleAction.Animation.AnimNotify.AttackState", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter);
IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(FAnimNotifyState_AttackWindowTest, FBattleActorTestFixture,"BattleAction.Animation.AnimNotifyState.AttackWindow", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter);

bool FAnimNotifyState_AttackWindowTest::RunTest(const FString& Parameters)
{
	if (!SetupWorld())
	{
		return false;
	}
	
	ON_SCOPE_EXIT{
		ClearWorld();
	};
	
	AActor* TestActor = GetWorld()->SpawnActor<AActor>();
	USkeletalMeshComponent* MeshComp = NewObject<USkeletalMeshComponent>(TestActor);
	UMockTestBattleCombatManagerComponent* CombatManagerComp = NewObject<UMockTestBattleCombatManagerComponent>(TestActor);
	
	TestActor->AddInstanceComponent(MeshComp);
	TestActor->AddInstanceComponent(CombatManagerComp);
	
	UAnimNotifyState_AttackWindow* AttackWindow = NewObject<UAnimNotifyState_AttackWindow>(GetWorld());
	
	TArray<FHitResult> HitResults;
	TArray<FHitResult> UniqueHitResults;
	
	AActor* HitActor_A = GetWorld()->SpawnActor<AActor>();
	USkeletalMeshComponent* HitActorMeshComp_A = NewObject<USkeletalMeshComponent>(HitActor_A);
	UCapsuleComponent* HitActorCapsuleComp_A = NewObject<UCapsuleComponent>(HitActor_A);
	
	FHitResult TestActor_CapsuleComponent_A(HitActor_A, HitActorCapsuleComp_A, FVector(10.f), FVector(20.f));
	FHitResult TestActor_SkeletalMeshComponent_A(HitActor_A, HitActorMeshComp_A, FVector(10.f), FVector(20.f));
	
	HitResults.Add(TestActor_CapsuleComponent_A);
	HitResults.Add(TestActor_SkeletalMeshComponent_A);
	
	AActor* HitActor_B = GetWorld()->SpawnActor<AActor>();
	USkeletalMeshComponent* HitActorMeshComp_B = NewObject<USkeletalMeshComponent>(HitActor_B);
	
	FHitResult TestActor_SkeletalMeshComponent_B(HitActor_B, HitActorMeshComp_B, FVector(10.f), FVector(20.f));
	HitResults.Add(TestActor_SkeletalMeshComponent_B);
	
	AttackWindow->MakeUniqueHitResults(HitResults, UniqueHitResults);
	
	TestEqual(TEXT("MakeUniqueHitResults는 중복을 제거한 HitResults만을 반환 해야 함"), UniqueHitResults.Num(),2);
	
	if (UniqueHitResults.Num() != 2)
	{
		return false;
	}
	
	TestEqual(TEXT("MakeUniqueHitResults가 진행되어도 HitResult의 Actor는 그대로 전달되어야 함"), UniqueHitResults[0].GetActor(), HitActor_A);
	TestEqual(TEXT("MakeUniqueHitResults가 진행되어도 HitResult의 Actor는 그대로 전달되어야 함"), UniqueHitResults[1].GetActor(), HitActor_B);
	
	AttackWindow->SendHitResults(UniqueHitResults, FPlatformTime::Seconds(), CombatManagerComp);
	
	TestEqual(TEXT("SendHitResults는 CombatManagerComp에게 HitResult를 전달해야함"), CombatManagerComp->HitEventCount, 1);
	
	return true;
}


bool FAnimNotify_AttackStateTest::RunTest(const FString& Parameters)
{
	if (!SetupWorld())
	{
		return false;
	}
	
	ON_SCOPE_EXIT{
		ClearWorld();
	};
	
	
	UAnimNotify_OpeningState* OpeningState = NewObject<UAnimNotify_OpeningState>(GetWorld());
	UAnimNotify_CommitState* CommitState = NewObject<UAnimNotify_CommitState>(GetWorld());
	UAnimNotify_EndingState* EndingState = NewObject<UAnimNotify_EndingState>(GetWorld());
	
	AActor* TestActor = GetWorld()->SpawnActor<AActor>();
	UAbilitySystemComponent* ASC = NewObject<UAbilitySystemComponent>(TestActor);
	TestActor->AddInstanceComponent(ASC);
	ASC->RegisterComponent();
	ASC->InitAbilityActorInfo(TestActor, TestActor);
	
	USkeletalMeshComponent* MeshComp = NewObject<USkeletalMeshComponent>(TestActor);
	
	OpeningState->BlockMovementTag(MeshComp);
	
	TestTrue(TEXT("OpeningState에서는 회전은 허용, 움직임은 막아야 함."),ASC->HasMatchingGameplayTag(FBattleGameplayTags::Get().Block_Movement_AllowRotation));
	
	CommitState->BlockMovementWithRotationTag(MeshComp);
	
	TestTrue(TEXT("CommitState에서는 회전도, 움직임도 막아야 함"), ASC->HasMatchingGameplayTag(FBattleGameplayTags::Get().Block_Movement));
	
	EndingState->AllowMovementTag(MeshComp);
	
	TestFalse(TEXT("EndingState를 지나면 움직임, 회전이 허용되어야 함"), ASC->HasMatchingGameplayTag(FBattleGameplayTags::Get().Block_Movement));
	TestFalse(TEXT("EndingState를 지나면 움직임이 허용되어야 함"), ASC->HasMatchingGameplayTag(FBattleGameplayTags::Get().Block_Movement_AllowRotation));
	
	return true;
}


bool FAnimNode_DestoryPartsTest::RunTest(const FString& Parameters)
{
	
	FAnimNode_DestroyedParts TestAnimNode_DestroyParts;
	
	TArray<FBoneTransform> BoneTransforms;
	TArray<FBoneInfo> BoneInfos;
	
	bool bSuccessMakeBones = false; 
	
	bSuccessMakeBones = TestAnimNode_DestroyParts.MakeBoneTransforms(BoneTransforms, BoneInfos);
	
	TestFalse(TEXT("BoneInfo에 아무것도 없으면, MakeBoneTransforms는 false를 리턴해야함"), bSuccessMakeBones);
	TestTrue(TEXT("BoneInfo에 아무것도 없으면, MakeBoneTransforms는 BoneTransforms에 아무것도 넣으면 안됨"),BoneTransforms.IsEmpty());
	
	FBoneInfo ZeroBoneInfo_A;
	FBoneInfo ZeroBoneInfo_B;
	
	FVector ZeroLocation = FVector::ZeroVector;
	
	ZeroBoneInfo_A.BoneIndex = 1;
	ZeroBoneInfo_B.BoneIndex = 1;
	
	ZeroBoneInfo_A.BoneTransform = FTransform(ZeroLocation);
	ZeroBoneInfo_B.BoneTransform = FTransform(ZeroLocation);
	
	BoneInfos.Add(ZeroBoneInfo_A);
	BoneInfos.Add(ZeroBoneInfo_B);
	
	bSuccessMakeBones = TestAnimNode_DestroyParts.MakeBoneTransforms(BoneTransforms, BoneInfos);
	
	TestFalse(TEXT("BoneInfo에 ZeroTransform만 있으면, MakeBoneTransforms는 false를 리턴해야함"), bSuccessMakeBones);
	TestTrue(TEXT("BoneInfo에 ZeroTransform만 있으면, MakeBoneTransforms는 BoneTransforms에 아무것도 넣으면 안됨"),BoneTransforms.IsEmpty());
	
	FBoneInfo NormalBoneInfo_A;
	FBoneInfo NormalBoneInfo_B;
	
	NormalBoneInfo_A.BoneIndex = 2;
	NormalBoneInfo_B.BoneIndex = 3;
	
	NormalBoneInfo_A.BoneTransform = FTransform(FVector(10.f));
	NormalBoneInfo_B.BoneTransform = FTransform(FVector(20.f));
	
	BoneInfos.Add(NormalBoneInfo_A);
	BoneInfos.Add(NormalBoneInfo_B);
	
	bSuccessMakeBones = TestAnimNode_DestroyParts.MakeBoneTransforms(BoneTransforms, BoneInfos);
	
	TestTrue(TEXT("BoneInfo에 제대로된 정보가 있으면, MakeBoneTransforms는 true를 리턴해야함"), bSuccessMakeBones);
	TestEqual(TEXT("BoneInfo에 제대로 된 정보가 2개 있으면, MakeBoneTransforms는 2개의 BoneTransform을 만들어야 함"), BoneTransforms.Num(), 2);
	
	
	return true;
}


#endif


