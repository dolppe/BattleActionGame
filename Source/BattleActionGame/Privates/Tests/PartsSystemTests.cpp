
#if WITH_DEV_AUTOMATION_TESTS
#include "PartsSystemTests.h"

#include "BattleActorTestFixture.h"
#include "NativeGameplayTags.h"
#include "BattleActionGame/Animation/DestroyedAnimInstance.h"
#include "BattleActionGame/Character/BattleCharacterBase.h"

IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(FPartsSystemManagerTest, FBattleActorTestFixture, "BattleAction.PartsSystem.PartsManager", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter);

UE_DEFINE_GAMEPLAY_TAG(TAG_PART_A, "TestPart.A");
UE_DEFINE_GAMEPLAY_TAG(TAG_PART_B, "TestPart.B");

bool FPartsSystemManagerTest::RunTest(const FString& Parameters)
{
	UWorld::InitializationValues NewIVS = UWorld::InitializationValues();
	NewIVS.bCreatePhysicsScene = true;
	SetInitializationValues(&NewIVS);
	
	if (!SetupWorld())
	{
		return false;
	}
	
	ON_SCOPE_EXIT{
		ClearWorld();
	};
	
	ABattleCharacterBase* TestCharacter = GetWorld()->SpawnActor<ABattleCharacterBase>();
	UMockTestPartsManagerComponent* PartsManagerComponent = NewObject<UMockTestPartsManagerComponent>(TestCharacter);
	
	TestCharacter->AddInstanceComponent(PartsManagerComponent);
	PartsManagerComponent->RegisterComponent();
	
	TMap<FGameplayTag, FPartData> OriginPartsData;
	
	// 데미지, 데미지, 파괴
	FPartData Part_A;
	
	FPartEventData PartEventA_A;
	PartEventA_A.TriggerHp = 3;
	PartEventA_A.EventType = EPartEventType::Damage;
	PartEventA_A.NextSurfaceType = SurfaceType6;
	
	FPartEventData PartEventA_B;
	PartEventA_B.TriggerHp = 1;
	PartEventA_B.EventType = EPartEventType::Damage;
	PartEventA_B.NextSurfaceType = SurfaceType7;
	
	FPartEventData PartEventA_C;
	PartEventA_C.TriggerHp = 0;
	PartEventA_C.EventType = EPartEventType::Destroy;
	
	Part_A.PartEvents.Add(PartEventA_A);
	Part_A.PartEvents.Add(PartEventA_B);
	Part_A.PartEvents.Add(PartEventA_C);
	Part_A.PartMaxHp = 5;
	Part_A.PartHp = 5;
	Part_A.SurfaceState = SurfaceType5;
	
	FPartData Part_B;
	
	FPartEventData PartEventB_A;
	PartEventB_A.TriggerHp = 0;
	PartEventB_A.EventType = EPartEventType::Destroy;
	
	Part_B.PartEvents.Add(PartEventB_A);
	Part_B.PartMaxHp = 5;
	Part_B.PartHp = 1;
	Part_B.SurfaceState = SurfaceType5;
	
	OriginPartsData.Add(TAG_PART_A, Part_A);
	OriginPartsData.Add(TAG_PART_B, Part_B);
	
	PartsManagerComponent->SetPartsData(OriginPartsData);
	
	FVector HitDirection = FVector(10.f);
	
	TMap<FGameplayTag, FPartData>& PartsData = PartsManagerComponent->GetPartsData();
	
	TestFalse(TEXT("현재 파츠 A의 체력은 5이고 1데미지를 받으면 4임, 다음 이벤트는 3이기에 False를 리턴해야함"),PartsManagerComponent->WillPartEventTrigger(TAG_PART_A));
	PartsManagerComponent->HandleDamagedToPart(TAG_PART_A, HitDirection);
	TestEqual(TEXT("파츠 A에 데미지를 줬기에 파츠 A의 Hp는 4여야 함"), PartsData[TAG_PART_A].PartHp, 4);
	TestTrue(TEXT("현재 파츠 A의 체력은 4이고 1데미지를 받으면 3임, 다음 이벤트는 3이기에 True 리턴해야함"),PartsManagerComponent->WillPartEventTrigger(TAG_PART_A));
	
	PartsManagerComponent->HandleDamagedToPart(TAG_PART_A, HitDirection);
	
	TestEqual(TEXT(""), PartsManagerComponent->GetSurfaceStateFromPart(TAG_PART_A), SurfaceType6);
	PartsManagerComponent->HandleDamagedToPart(TAG_PART_A, HitDirection);
	PartsManagerComponent->HandleDamagedToPart(TAG_PART_A, HitDirection);
	TestEqual(TEXT(""), PartsManagerComponent->GetSurfaceStateFromPart(TAG_PART_A), SurfaceType7);
	
	PartsManagerComponent->SetDestroyedAnimInstance(UDestroyedAnimInstance::StaticClass());
	PartsManagerComponent->HandleDamagedToPart(TAG_PART_A, HitDirection);
	
	TArray<ABrokenPartActor*>& DestroyedPartsArr = PartsManagerComponent->GetDestroyedParts();
	TArray<FGameplayTag>& AppliedPartTagsArr = PartsManagerComponent->GetAppliedPartTags();
	TArray<FGameplayTag>& DestroyedPartTagsArr = PartsManagerComponent->GetDestroyedPartTags();
	
	TestEqual(TEXT("현재 하나의 부위가 파괴되어 하나의 Parts만 들어가 있어야 함"), DestroyedPartsArr.Num(), 1);
	TestEqual(TEXT("현재 하나의 부위가 파괴되어 하나의 PartTag만 들어가 있어야 함"), AppliedPartTagsArr.Num(), 1);
	TestEqual(TEXT("현재 하나의 부위가 파괴되어 하나의 PartTag만 들어가 있어야 함"), DestroyedPartTagsArr.Num(), 1);
	
	if (DestroyedPartsArr.Num() != 1 || AppliedPartTagsArr.Num() != 1 || DestroyedPartTagsArr.Num() != 1)
	{
		return false;
	}
	
	TestTrue(TEXT("파괴된 부위의 Tag를 가지고 있어야 함"), AppliedPartTagsArr[0] == TAG_PART_A);
	TestTrue(TEXT("파괴된 부위의 Tag를 가지고 있어야 함"), DestroyedPartTagsArr[0] == TAG_PART_A);
	
	PartsManagerComponent->HandleDamagedToPart(TAG_PART_B, HitDirection);
	
	TestEqual(TEXT("현재 두 개의 부위가 파괴되어 두 개의 Parts만 들어가 있어야 함"), DestroyedPartsArr.Num(), 2);
	TestEqual(TEXT("현재 두 개의 부위가 파괴되어 두 개의 PartTag만 들어가 있어야 함"), AppliedPartTagsArr.Num(), 2);
	TestEqual(TEXT("현재 두 개의 부위가 파괴되어 두 개의 PartTag만 들어가 있어야 함"), DestroyedPartTagsArr.Num(), 2);
	
	if (DestroyedPartsArr.Num() != 2 || AppliedPartTagsArr.Num() != 2 || DestroyedPartTagsArr.Num() != 2)
	{
		return false;
	}
	
	TestTrue(TEXT("파괴된 부위의 Tag를 가지고 있어야 함"), AppliedPartTagsArr[1] == TAG_PART_B);
	TestTrue(TEXT("파괴된 부위의 Tag를 가지고 있어야 함"), DestroyedPartTagsArr[1] == TAG_PART_B);
	
	
	return true;
	
	
}


#endif

