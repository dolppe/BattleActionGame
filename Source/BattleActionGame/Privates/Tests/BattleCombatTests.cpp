#if WITH_DEV_AUTOMATION_TESTS



#include "BattleCombatTests.h"

#include "BattleActorTestFixture.h"
#include "BattleActionGame/BattleGameplayTags.h"
#include "BattleActionGame/AbilitySystem/BattleAbilitySystemComponent.h"
#include "BattleActionGame/Character/BattleCharacterBase.h"
#include "BattleActionGame/Combat/Item/BattleItemData.h"
#include "BattleActionGame/Player/BattlePlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BattleCombatTests)

IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(FGameplayAbility_AttackTest, FBattleActorTestFixture, "BattleAction.Combat.GAAttack", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter);
IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(FQuickBarComponentTest, FBattleFullActorTestFixture, "BattleAction.Combat.QuickBarComponent", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter);

bool FQuickBarComponentTest::RunTest(const FString& Parameters)
{
	if (!SetupWorld())
	{
		return false;
	}
	
	ON_SCOPE_EXIT{
		ClearWorld();
	};
	
	ABattlePlayerController* TestController = GetWorld()->SpawnActor<ABattlePlayerController>();
	UBattleQuickBarComponent* QuickBarComponent = NewObject<UBattleQuickBarComponent>(TestController);
	
	TestController->AddInstanceComponent(QuickBarComponent);
	QuickBarComponent->RegisterComponent();
	
	QuickBarComponent->BeginPlay();
	
	UBattleItemData* TestItem_A = NewObject<UMockBattleItemData_A>(GetWorld());
	UBattleItemData* TestItem_B = NewObject<UMockBattleItemData_B>(GetWorld());
	UBattleItemData* TestItem_C = NewObject<UMockBattleItemData_C>(GetWorld());
	UBattleItemData* TestItem_D = NewObject<UMockBattleItemData_D>(GetWorld());
	QuickBarComponent->AddItemToSlot(0, TestItem_A, 2);
	QuickBarComponent->AddItemToSlot(1, TestItem_B, 2);
	QuickBarComponent->AddItemToSlot(2, TestItem_C, 2);
	QuickBarComponent->AddItemToSlot(4, TestItem_D, 2);

	
	TestEqual(TEXT("Item 추가가 올바르게 진행되어야 함"), QuickBarComponent->GetItemSlotIndex(TestItem_A), 0);
	TestEqual(TEXT("Item 추가가 올바르게 진행되어야 함"), QuickBarComponent->GetItemSlotIndex(TestItem_B), 1);
	TestEqual(TEXT("Item 추가가 올바르게 진행되어야 함"), QuickBarComponent->GetItemSlotIndex(TestItem_C), 2);
	TestEqual(TEXT("없는 Idx에 Item 추가 시도는 수행되지 않음"), QuickBarComponent->GetItemSlotIndex(TestItem_D), -1);
	
	QuickBarComponent->AddItemToSlot(3, TestItem_D, 2);
	
	TestFalse(TEXT("현재 존재하는 수량보다 과한 요구는 사용되지 않음"),QuickBarComponent->UseItemQuantity(30));
	TestTrue(TEXT("현재 존재하는 수량보다 작은 요구는 사용됨"), QuickBarComponent->UseItemQuantity(1));
	TestEqual(TEXT("2개중 1개를 썼으니 1개가 남아야 함"), QuickBarComponent->GetSlotsItem(0).Num, 1);
	
	QuickBarComponent->SetActiveSlotIndex(1);
	
	TestTrue(TEXT("2개 중 2개는 사용 가능 해야함"), QuickBarComponent->UseItemQuantity(2));
	TestEqual(TEXT("현재 Active중인 Item을 다 쓰면 ActiveIdx가 0으로 변경 됨"), QuickBarComponent->GetActiveSlotIdx(), 0);
	
	QuickBarComponent->CycleActiveSlotForward();
	TestEqual(TEXT("1이 존재하지 않기에 CycleForward하면 0 => 2로 감"), QuickBarComponent->GetActiveSlotIdx(), 2);
	
	QuickBarComponent->CycleActiveSlotForward();
	TestEqual(TEXT("CycleForward하면 2 => 3로 감"), QuickBarComponent->GetActiveSlotIdx(), 3);
	
	QuickBarComponent->CycleActiveSlotForward();
	TestEqual(TEXT("최대 Slot의 개수가 4로 설정되어 있기에 CycleForward하면 3 => 0로 감"), QuickBarComponent->GetActiveSlotIdx(), 0);
	
	QuickBarComponent->CycleActiveSlotBackward();
	TestEqual(TEXT("최대 Slot의 개수가 4로 설정되어 있기에 CycleBackward하면 0 => 3로 감"), QuickBarComponent->GetActiveSlotIdx(), 3);
	
	
	
	return true;
}


bool FGameplayAbility_AttackTest::RunTest(const FString& Parameters)
{
	if (!SetupWorld())
	{
		return false;
	}
	
	ON_SCOPE_EXIT{
		ClearWorld();
	};
	
	ABattleCharacterBase* AttackActor = GetWorld()->SpawnActor<ABattleCharacterBase>();
	AActor* HitTestActor_A = GetWorld()->SpawnActor<AActor>();
	AActor* HitTestActor_B = GetWorld()->SpawnActor<AActor>();
	AActor* HitTestActor_C = GetWorld()->SpawnActor<AActor>();
	
	UBattleAbilitySystemComponent* ASC = NewObject<UBattleAbilitySystemComponent>(AttackActor);
	AttackActor->AddInstanceComponent(ASC);
	ASC->RegisterComponent();
	ASC->InitAbilityActorInfo(AttackActor, AttackActor);
	
	
	FGameplayAbilitySpec GASpec(UMockBattleGameplayAbility_Attack_Parent::StaticClass());
	FGameplayAbilitySpecHandle SpecHandle= ASC->GiveAbility(GASpec);
	
	ASC->TryActivateAbility(SpecHandle);
	
	FGameplayAbilitySpec* InstancedSpec = ASC->FindAbilitySpecFromHandle(SpecHandle);
	UMockBattleGameplayAbility_Attack_Parent* InstancedGA = Cast<UMockBattleGameplayAbility_Attack_Parent>(InstancedSpec->GetPrimaryInstance());
	
	TArray<FHitResult> HitResults;
	FHitResult HitResult_A;
	HitResult_A.HitObjectHandle = FActorInstanceHandle(HitTestActor_A);
	HitResults.Add(HitResult_A);
	
	FHitResult HitResult_B;
	HitResult_B.HitObjectHandle = FActorInstanceHandle(HitTestActor_B);
	HitResults.Add(HitResult_B);
	
	FBattleHitMessage HitMessage_AB;
	
	HitMessage_AB.WindowIndex = 0;
	HitMessage_AB.HitResults = HitResults;
	
	InstancedGA->AttackHitConfirm(HitMessage_AB);
	
	TestEqual(TEXT("HitMessage에 HitResult가 2개 갔으니 AlreadyHitActors의 수도 2여야 함"), InstancedGA->GetAlreadyHitActors().Num(), 2);
	TestEqual(TEXT("2개의 HitResult가 전달됐으니 OnTargetDataReadyCallback도 2번 진행되어야 함"), InstancedGA->TargetDataCount, 2);
	
	if (InstancedGA->CurTargetDataLists.Num() != 2)
	{
		return false;
	}
	TestEqual(TEXT("AlreadyHitActors와 TargetData로 전달된 Actor는 같아야함"), InstancedGA->CurTargetDataLists[0], InstancedGA->GetAlreadyHitActors()[0]);
	TestEqual(TEXT("AlreadyHitActors와 TargetData로 전달된 Actor는 같아야함"), InstancedGA->CurTargetDataLists[1], InstancedGA->GetAlreadyHitActors()[1]);
	
	FHitResult HitResult_C;
	HitResult_C.HitObjectHandle = FActorInstanceHandle(HitTestActor_C);
	
	HitResults.Empty();
	HitResults.Add(HitResult_A);
	HitResults.Add(HitResult_C);
	
	FBattleHitMessage HitMessage_AC;
	
	HitMessage_AC.WindowIndex = 0;
	HitMessage_AC.HitResults = HitResults;
	
	InstancedGA->AttackHitConfirm(HitMessage_AC);
	
	TestEqual(TEXT("A는 이미 AlreadyHitActors에 포함되어 있기에 C만 추가해서 3개여야 함"), InstancedGA->GetAlreadyHitActors().Num(), 3);
	TestEqual(TEXT("이미 A는 추가되어 있기에 OnTargetDataReadyCallback 호출도 한번만 진행됨"), InstancedGA->TargetDataCount, 3);
	
	if (InstancedGA->CurTargetDataLists.Num() != 3)
	{
		return false;
	}
	TestEqual(TEXT("AlreadyHitActors와 TargetData로 전달된 Actor는 같아야함"), InstancedGA->CurTargetDataLists[0], InstancedGA->GetAlreadyHitActors()[0]);
	TestEqual(TEXT("AlreadyHitActors와 TargetData로 전달된 Actor는 같아야함"), InstancedGA->CurTargetDataLists[1], InstancedGA->GetAlreadyHitActors()[1]);
	TestEqual(TEXT("AlreadyHitActors와 TargetData로 전달된 Actor는 같아야함"), InstancedGA->CurTargetDataLists[2], InstancedGA->GetAlreadyHitActors()[2]);
	
	HitResults.Empty();
	HitResults.Add(HitResult_C);
	
	FBattleHitMessage HitMessage_C;
	
	HitMessage_C.WindowIndex = 1;
	HitMessage_C.HitResults = HitResults;
	
	InstancedGA->TargetDataCount = 0;
	InstancedGA->CurTargetDataLists.Empty();
	
	InstancedGA->AttackHitConfirm(HitMessage_C);
	
	TestEqual(TEXT("새로운 WindowIndex가 도달했기에 전의 AlreadyHitActors의 리스트는 다 제거되고 새로 쌓임"), InstancedGA->GetAlreadyHitActors().Num(), 1);
	TestEqual(TEXT("하나의 HitResult만 도달했기에 한번만 실행됨"), InstancedGA->TargetDataCount, 1);
	if (InstancedGA->CurTargetDataLists.Num() != 1)
	{
		return false;
	}
	TestEqual(TEXT("AlreadyHitActors와 TargetData로 전달된 Actor는 같아야함"), InstancedGA->CurTargetDataLists[0], InstancedGA->GetAlreadyHitActors()[0]);
	
	InstancedGA->EndAbility(SpecHandle, ASC->AbilityActorInfo.Get(), InstancedSpec->ActivationInfo, true, false);
	
	TestEqual(TEXT("AttackGA가 끝나면 Status Tag도 사라져야함"),ASC->GetGameplayTagCount(FBattleGameplayTags::Get().Status_Action_Attack), 0);
	TestEqual(TEXT("AttackGA가 끝나면 BlockMovement Tag도 사라져야함"),ASC->GetGameplayTagCount(FBattleGameplayTags::Get().Block_Movement), 0);
	TestEqual(TEXT("AttackGA가 끝나면 BlockMovement_AllowRotation Tag도 사라져야함"),ASC->GetGameplayTagCount(FBattleGameplayTags::Get().Block_Movement_AllowRotation), 0);
	
	
	return true;
}


#endif

UMockBattleGameplayAbility_Attack_Parent::UMockBattleGameplayAbility_Attack_Parent(
	const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	
}

void UMockTestBattleCombatManagerComponent::OnHitEvent(const FBattleHitMessage& HitMessage)
{
	HitEventCount++;
	Super::OnHitEvent(HitMessage);
}


