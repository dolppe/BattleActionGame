
#if WITH_DEV_AUTOMATION_TESTS

#include "GameplayAbilityTests.h"

#include "AbilitySystemComponent.h"
#include "BattleActorTestFixture.h"
#include "NativeGameplayTags.h"
#include "BattleActionGame/BattleGameplayTags.h"
#include "BattleActionGame/AbilitySystem/BattleAbilitySet.h"
#include "BattleActionGame/AbilitySystem/BattleAbilitySystemComponent.h"
#include "BattleActionGame/AbilitySystem/BattleAbilityTagRelationshipMapping.h"
#include "BattleActionGame/AbilitySystem/BattleGlobalAbilitySystem.h"
#include "BattleActionGame/AbilitySystem/Abilities/BattleGameplayAbility_ConvertToAI.h"
#include "BattleActionGame/AbilitySystem/Abilities/BattleGameplayAbility_Death.h"
#include "BattleActionGame/AbilitySystem/Abilities/Phase/BattleGamePhaseSubsystem.h"
#include "BattleActionGame/AbilitySystem/Attributes/BattleCombatSet.h"
#include "BattleActionGame/AbilitySystem/Attributes/BattleEnemySet.h"
#include "BattleActionGame/AbilitySystem/Attributes/BattleHealthSet.h"
#include "BattleActionGame/AbilitySystem/Executions/BattleDamageExecution.h"
#include "BattleActionGame/Character/BattleCharacter.h"
#include "BattleActionGame/Character/BattleHealthComponent.h"
#include "BattleActionGame/Character/BattleHeroComponent.h"
#include "BattleActionGame/Character/BattleObserverPawn.h"
#include "BattleActionGame/Character/BattlePawnExtensionComponent.h"
#include "BattleActionGame/Combat/BattleGameplayAbility_ComboAttack.h"
#include "BattleActionGame/Combat/BattleGameplayAbility_ComboStrongAttack.h"
#include "BattleActionGame/Enemy/BattleEnemyData.h"
#include "BattleActionGame/GameModes/BattleGameMode.h"
#include "BattleActionGame/GameModes/BattleGameState.h"
#include "BattleActionGame/Player/BattlePlayerAIController.h"
#include "BattleActionGame/Player/BattlePlayerController.h"
#include "BattleActionGame/System/BattleGameInstance.h"
#include "Engine/GameEngine.h"
#include "GameFramework/GameMode.h"

IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(FPhaseSystemTest, FBattleActorTestFixture, "BattleAction.AbilitySystem.PhaseSubsystem", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter);
IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(FGameplayAbility_ConvertAITest, FBattleActorTestFixture, "BattleAction.AbilitySystem.Ability.ConvertToAI", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter);

IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(FGameplayAbility_DeathTest, FBattleActorTestFixture, "BattleAction.AbilitySystem.Ability.Death", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter);
DEFINE_LATENT_AUTOMATION_COMMAND_THREE_PARAMETER(FGameplayAbility_DeathTest_WaitEvent, FAutomationTestBase*, Test, double, StartTime, UBattleHealthComponent*, HealthComponent);

IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(FCombatAttributeSetTest, FBattleActorTestFixture, "BattleAction.AbilitySystem.Attribute.CombatSet", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter);
IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(FEnemyAttributeSetTest, FBattleActorTestFixture, "BattleAction.AbilitySystem.Attribute.EnemySet", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter);
IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(FHealthAttributeSetTest, FBattleActorTestFixture, "BattleAction.AbilitySystem.Attribute.HealthSet", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter);
IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(FDamageExecutionTest, FBattleActorTestFixture, "BattleAction.AbilitySystem.Execution.DamageExecution", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter);

IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(FAbilitySetTest, FBattleActorTestFixture, "BattleAction.AbilitySystem.AbilitySet", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter);
IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(FBattleAbilitySystemComponentTest, FBattleActorTestFixture, "BattleAction.AbilitySystem.AbilitySystemComponent", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter);
IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(FAbilityTagRelationshipMappingTest, FBattleActorTestFixture, "BattleAction.AbilitySystem.AbilityTagRelationshipMapping", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter);

IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(FGlobalAbilitySystemTest, FBattleActorTestFixture, "BattleAction.AbilitySystem.GlobalAbilitySystem", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter);

UE_DEFINE_GAMEPLAY_TAG(FPhaseTag_A, "Test.Phase.A");
UE_DEFINE_GAMEPLAY_TAG(FPhaseTag_B, "Test.Phase.B");
UE_DEFINE_GAMEPLAY_TAG(FPhaseTag_A_C, "Test.Phase.A.C");

UE_DEFINE_GAMEPLAY_TAG(FGATag_A, "Test.GA.A");
UE_DEFINE_GAMEPLAY_TAG(FGATag_B, "Test.GA.B");
UE_DEFINE_GAMEPLAY_TAG(FGATag_C, "Test.GA.C");

UE_DEFINE_GAMEPLAY_TAG(FStatus_A, "Test.Status.A");
UE_DEFINE_GAMEPLAY_TAG(FStatus_B, "Test.Status.B");

bool FGlobalAbilitySystemTest::RunTest(const FString& Parameters)
{
	if (!SetupWorld())
	{
		return false;
	}
	
	ON_SCOPE_EXIT{
		ClearWorld();
	};
	
	
	UBattleGlobalAbilitySystem* BattleGlobalAbilitySystem = Cast<UBattleGlobalAbilitySystem>(GetWorld()->GetSubsystem<UBattleGlobalAbilitySystem>());
	
	AActor* Actor_A = GetWorld()->SpawnActor<AActor>();
	UBattleAbilitySystemComponent* ASC_A = NewObject<UBattleAbilitySystemComponent>(Actor_A);
	Actor_A->AddInstanceComponent(ASC_A);
	ASC_A->RegisterComponent();
	ASC_A->InitAbilityActorInfo(Actor_A, Actor_A);
	
	AActor* Actor_B = GetWorld()->SpawnActor<AActor>();
	UBattleAbilitySystemComponent* ASC_B = NewObject<UBattleAbilitySystemComponent>(Actor_B);
	Actor_B->AddInstanceComponent(ASC_B);
	ASC_B->RegisterComponent();
	ASC_B->InitAbilityActorInfo(Actor_B, Actor_B);
	
	BattleGlobalAbilitySystem->RegisterASC(ASC_A);
	BattleGlobalAbilitySystem->RegisterASC(ASC_B);
	
	TSubclassOf<UGameplayAbility> GAClass = UMockTestGameplayAbilityBase::StaticClass();
	
	FGameplayAbilitySpec* GASpec_A = ASC_A->FindAbilitySpecFromClass(GAClass);
	FGameplayAbilitySpec* GASpec_B = ASC_B->FindAbilitySpecFromClass(GAClass);
	
	TestNull(TEXT("GiveAbility를 하기 전에는 Spec이 null이어야 함"), GASpec_A);
	TestNull(TEXT("GiveAbility를 하기 전에는 Spec이 null이어야 함"), GASpec_B);
	
	BattleGlobalAbilitySystem->GiveAbilityToAll(GAClass);
	
	GASpec_A = ASC_A->FindAbilitySpecFromClass(GAClass);
	GASpec_B = ASC_B->FindAbilitySpecFromClass(GAClass);
	
	TestNotNull(TEXT("GlobalSystem에 등록된 ASC는 모두 GiveAbility를 실행 해야함"), GASpec_A);
	TestNotNull(TEXT("GlobalSystem에 등록된 ASC는 모두 GiveAbility를 실행 해야함"), GASpec_B);
	
	if (GASpec_A == nullptr || GASpec_B == nullptr)
	{
		return false;
	}
	
	UMockTestGameplayAbilityBase* GA_A = Cast<UMockTestGameplayAbilityBase>(GASpec_A->GetPrimaryInstance());
	UMockTestGameplayAbilityBase* GA_B = Cast<UMockTestGameplayAbilityBase>(GASpec_B->GetPrimaryInstance());
	
	BattleGlobalAbilitySystem->TryAbilityToAll(GAClass);
	
	TestEqual(TEXT("GlobalSystem에 등록된 ASC는 모두 Ability를 실행 해야함"), GA_A->ActivateCount, 1);
	TestEqual(TEXT("GlobalSystem에 등록된 ASC는 모두 Ability를 실행 해야함"), GA_B->ActivateCount, 1);
	
	BattleGlobalAbilitySystem->RemoveAbilityFromAll(GAClass);
	
	GASpec_A = ASC_A->FindAbilitySpecFromClass(GAClass);
	GASpec_B = ASC_B->FindAbilitySpecFromClass(GAClass);
	
	TestNull(TEXT("RemoveAbility가 진행되면 ActivatableAbilities에서 제외되어야 함"), GASpec_A);
	TestNull(TEXT("RemoveAbility가 진행되면 ActivatableAbilities에서 제외되어야 함"), GASpec_B);
	
	TSubclassOf<UGameplayEffect> GEClass = UMockGameplayEffect_A::StaticClass();
	
	BattleGlobalAbilitySystem->ApplyEffectToAll(GEClass);
	
	FGameplayEffectQuery Query;
	Query.EffectDefinition = GEClass;
	
	TArray<FActiveGameplayEffectHandle> GE_Handles_A = ASC_A->GetActiveEffects(Query);
	TArray<FActiveGameplayEffectHandle> GE_Handles_B = ASC_B->GetActiveEffects(Query);
	
	TestEqual(TEXT("GlobalSystem에 등록된 ASC는 모두 GE를 적용 해야함"), GE_Handles_A.Num(), 1);
	TestEqual(TEXT("GlobalSystem에 등록된 ASC는 모두 GE를 적용 해야함"), GE_Handles_B.Num(), 1);
	
	if (GE_Handles_A.Num() != 1 || GE_Handles_B.Num() != 1)
	{
		return false;
	}
	
	BattleGlobalAbilitySystem->RemoveEffectFromAll(GEClass);
	
	GE_Handles_A = ASC_A->GetActiveEffects(Query);
	GE_Handles_B = ASC_B->GetActiveEffects(Query);
	
	TestEqual(TEXT("GlobalSystem에 등록된 ASC는 모두 GE가 Remove 되어야 함"), GE_Handles_A.Num(), 0);
	TestEqual(TEXT("GlobalSystem에 등록된 ASC는 모두 GE가 Remove 되어야 함"), GE_Handles_B.Num(), 0);
	
	
	
	
	return true;
}

bool FAbilityTagRelationshipMappingTest::RunTest(const FString& Parameters)
{
	if (!SetupWorld())
	{
		return false;
	}
	
	ON_SCOPE_EXIT{
		ClearWorld();
	};
	
	AActor* Actor = GetWorld()->SpawnActor<AActor>();
	UBattleAbilitySystemComponent* ASC = NewObject<UBattleAbilitySystemComponent>(Actor);
	Actor->AddInstanceComponent(ASC);
	ASC->RegisterComponent();
	ASC->InitAbilityActorInfo(Actor, Actor);
	
	FGameplayAbilitySpec GASpec_A(UMockTestGameplayAbility_A::StaticClass());
	FGameplayAbilitySpecHandle GASpecHandle_A = ASC->GiveAbility(GASpec_A);
	FGameplayAbilitySpec* CreatedSpec_A = ASC->FindAbilitySpecFromHandle(GASpecHandle_A);
	UMockTestGameplayAbilityBase* GA_A = Cast<UMockTestGameplayAbilityBase>(CreatedSpec_A->GetPrimaryInstance());
	
	FGameplayAbilitySpec GASpec_B(UMockTestGameplayAbility_B::StaticClass());
	FGameplayAbilitySpecHandle GASpecHandle_B = ASC->GiveAbility(GASpec_B);
	FGameplayAbilitySpec* CreatedSpec_B = ASC->FindAbilitySpecFromHandle(GASpecHandle_B);
	UMockTestGameplayAbilityBase* GA_B = Cast<UMockTestGameplayAbilityBase>(CreatedSpec_B->GetPrimaryInstance());
	
	FGameplayAbilitySpec GASpec_C(UMockTestGameplayAbility_C::StaticClass());
	FGameplayAbilitySpecHandle GASpecHandle_C = ASC->GiveAbility(GASpec_C);
	FGameplayAbilitySpec* CreatedSpec_C = ASC->FindAbilitySpecFromHandle(GASpecHandle_C);
	UMockTestGameplayAbilityBase* GA_C = Cast<UMockTestGameplayAbilityBase>(CreatedSpec_C->GetPrimaryInstance());
	
	UBattleAbilityTagRelationshipMappingForTest* TagRelationshipMapping = NewObject<UBattleAbilityTagRelationshipMappingForTest>();
	
	FBattleAbilityTagRelationship TagRelationship_A;
	TagRelationship_A.AbilityTag = FGATag_A;
	TagRelationship_A.ActivationBlockedTags.AddTag(FStatus_A);
	TagRelationship_A.AbilityTagsToBlock.AddTag(FGATag_C);
	
	FBattleAbilityTagRelationship TagRelationship_B;
	TagRelationship_B.AbilityTag = FGATag_B;
	TagRelationship_B.ActivationRequiredTags.AddTag(FStatus_B);
	TagRelationship_B.AbilityTagsToCancel.AddTag(FGATag_A);
	
	TagRelationshipMapping->AddTagRelationship(TagRelationship_A);
	TagRelationshipMapping->AddTagRelationship(TagRelationship_B);
	
	ASC->SetTagRelationshipMapping(TagRelationshipMapping);
	
	ASC->AddLooseGameplayTag(FStatus_A);
	ASC->TryActivateAbility(GASpecHandle_A);
	
	TestEqual(TEXT("ActivationBlockedTag인 Tag가 활성화 되어 있으면 GA 실행 불가능"), GA_A->ActivateCount, 0);
	ASC->RemoveLooseGameplayTag(FStatus_A);
	ASC->TryActivateAbility(GASpecHandle_A);
	TestEqual(TEXT("ActivationBlockedTag인 Tag가 비활성화 되면 GA 실행이 가능"), GA_A->ActivateCount, 1);
	
	ASC->TryActivateAbility(GASpecHandle_C);
	TestEqual(TEXT("AbilityTagsToBlock을 가진 GA가 활성화 중이면, 해당 Tag를 가진 GA는 활성화 불가능"), GA_C->ActivateCount, 0);
	
	ASC->TryActivateAbility(GASpecHandle_B);
	TestEqual(TEXT("ActivationRequiredTag인 Tag가 없으면 GA 실행이 불가능"), GA_B->ActivateCount, 0);
	ASC->AddLooseGameplayTag(FStatus_B);
	ASC->TryActivateAbility(GASpecHandle_B);
	TestEqual(TEXT("ActivationRequiredTag인 Tag가 활성화 되면 GA 실행이 가능"), GA_B->ActivateCount, 1);
	TestEqual(TEXT("AbilityTagsToCancel을 가진 GA가 활성화 되면, 해당 Tag를 가진 GA는 Cancel이 되어야 함"), GA_A->EndAbilityCount, 1);
	
	ASC->TryActivateAbility(GASpecHandle_C);
	TestEqual(TEXT("AbilityTagsToBlock을 가진 GA가 종료되면, 해당 Tag를 가진 GA는 실행 가능."), GA_C->ActivateCount, 1);
	
	
	
	return true;
}

bool FBattleAbilitySystemComponentTest::RunTest(const FString& Parameters)
{
	if (!SetupWorld())
	{
		return false;
	}
	
	ON_SCOPE_EXIT{
		ClearWorld();
	};
	
	AActor* Actor = GetWorld()->SpawnActor<AActor>();
	UBattleAbilitySystemComponent* ASC = NewObject<UBattleAbilitySystemComponent>(Actor);
	Actor->AddInstanceComponent(ASC);
	ASC->RegisterComponent();
	ASC->InitAbilityActorInfo(Actor, Actor);
	
	FGameplayAbilitySpec TriggeredSpec(UMockGameplayAbility_InputTriggered::StaticClass());
	TriggeredSpec.DynamicAbilityTags.AddTag(FGATag_A);
	
	FGameplayAbilitySpecHandle TriggeredSpecHandle = ASC->GiveAbility(TriggeredSpec);
	FGameplayAbilitySpec* CreatedTriggeredSpec = ASC->FindAbilitySpecFromHandle(TriggeredSpecHandle);
	UMockTestGameplayAbilityBase* TriggeredGA = Cast<UMockTestGameplayAbilityBase>(CreatedTriggeredSpec->GetPrimaryInstance());
	
	FGameplayAbilitySpec HeldSpec(UMockGameplayAbility_WhileInputActive::StaticClass());
	HeldSpec.DynamicAbilityTags.AddTag(FGATag_B);
	
	FGameplayAbilitySpecHandle HeldSpecHandle = ASC->GiveAbility(HeldSpec);
	FGameplayAbilitySpec* CreatedHeldSpec = ASC->FindAbilitySpecFromHandle(HeldSpecHandle);
	UMockTestGameplayAbilityBase* HeldGA = Cast<UMockTestGameplayAbilityBase>(CreatedHeldSpec->GetPrimaryInstance());
	
	ASC->AbilityInputTagPressed(FGATag_A);
	ASC->AbilityInputTagPressed(FGATag_B);
	
	ASC->ProcessAbilityInput(1.f, false);
	
	TestEqual(TEXT("Ability Input이 Pressed되면,Triggered는 실행되어야 함"), TriggeredGA->ActivateCount, 1);
	TestTrue(TEXT("Ability Input이 Pressed되면,Triggered Spec의 InputPressed는 true가 되어야 함"), CreatedTriggeredSpec->InputPressed);
	TestEqual(TEXT("Ability Input이 Pressed되면,WhileInputActive는 실행되어야 함"), HeldGA->ActivateCount, 1);
	TestTrue(TEXT("Ability Input이 Pressed되면,WhileInputActive Spec의 InputPressed는 true가 되어야 함"), CreatedHeldSpec->InputPressed);
	
	ASC->AbilityInputTagPressed(FGATag_A);
	ASC->ProcessAbilityInput(1.f, false);
	
	TestEqual(TEXT("Ability Input이 다시 Pressed되면,Triggered는 InputPressed Event가 실행되어야 함"), TriggeredGA->InputPressedCount, 1);
	
	ASC->AbilityInputTagReleased(FGATag_A);
	ASC->ProcessAbilityInput(1.f, false);
	
	TestFalse(TEXT("Released가 되면 Spec의 InputPresssed가 false가 되어야 함"), CreatedTriggeredSpec->InputPressed);
	TestEqual(TEXT("Released가 되면 Ability에 InputReleased Event가 실행되어야 함"), TriggeredGA->InputReleasedCount, 1);
	
	ASC->AddLooseGameplayTag(TAG_Gameplay_AbilityInputBlocked);
	ASC->AbilityInputTagPressed(FGATag_A);
	ASC->ProcessAbilityInput(1.f,false);
	
	TestEqual(TEXT("Gameplay.AbilityInputBlockedTag가 있으면, Input 입력이 무시되어야 함"), TriggeredGA->InputPressedCount, 1);
	ASC->RemoveLooseGameplayTag(TAG_Gameplay_AbilityInputBlocked);
	
	ASC->CancelAbilityByFunc([TriggeredSpecHandle](const UBattleGameplayAbility* BattleAbility, FGameplayAbilitySpecHandle Handle)
	{
		return Handle == TriggeredSpecHandle;
	}, true);
	
	TestEqual(TEXT("CancelAbilityByFunc의 조건에 부합되는 GA만 Cancel되어야 함, TriggeredGA는 Handle이 같기에 종료됨"), TriggeredGA->EndAbilityCount, 1);
	TestEqual(TEXT("CancelAbilityByFunc의 조건에 부합되는 GA만 Cancel되어야 함, HeldGA는 Handle이 다르기에 종료되지 않음"), HeldGA->EndAbilityCount, 0);
	
	
	return true;
}

bool FAbilitySetTest::RunTest(const FString& Parameters)
{
	if (!SetupWorld())
	{
		return false;
	}
	
	ON_SCOPE_EXIT{
		ClearWorld();
	};
	
	AActor* Actor = GetWorld()->SpawnActor<AActor>();
	UBattleAbilitySystemComponent* ASC = NewObject<UBattleAbilitySystemComponent>(Actor);
	Actor->AddInstanceComponent(ASC);
	ASC->RegisterComponent();
	ASC->InitAbilityActorInfo(Actor, Actor);
	
	UBattleAbilitySetForTest* AbilitySetData = NewObject<UBattleAbilitySetForTest>();
	
	FBattleAbilitySet_GameplayAbility GATest_A;
	FBattleAbilitySet_GameplayAbility GATest_B;
	
	GATest_A.Ability = UMockGameplayAbility_WhileInputActive::StaticClass();
	GATest_B.Ability = UMockGameplayAbility_InputTriggered::StaticClass();
	
	AbilitySetData->AddGameplayAbility(GATest_A);
	AbilitySetData->AddGameplayAbility(GATest_B);
	
	FBattleAbilitySet_GameplayEffect GETest_A;
	FBattleAbilitySet_GameplayEffect GETest_B;
	
	GETest_A.GameplayEffect = UMockGameplayEffect_A::StaticClass();
	GETest_B.GameplayEffect = UMockGameplayEffect_B::StaticClass();
	
	AbilitySetData->AddGameplayEffect(GETest_A);
	AbilitySetData->AddGameplayEffect(GETest_B);
	
	FBattleAbilitySet_AttributeSet AttributeSetTest_A;
	FBattleAbilitySet_AttributeSet AttributeSetTest_B;
	
	AttributeSetTest_A.AttributeSet = UMockAttributeSet_A::StaticClass();
	AttributeSetTest_B.AttributeSet = UMockAttributeSet_B::StaticClass();
	
	AbilitySetData->AddAttributeSet(AttributeSetTest_A);
	AbilitySetData->AddAttributeSet(AttributeSetTest_B);
	
	FBattleAbilitySet_GrantedHandles GrantedHandles;
	
	AbilitySetData->GiveToAbilitySystem(ASC, &GrantedHandles);
	
	TestNotNull(TEXT("AbilitySet에 등록된 Ability는 GiveAbility가 완료된 상태여야 함"),ASC->FindAbilitySpecFromClass(UMockGameplayAbility_WhileInputActive::StaticClass()));
	TestNotNull(TEXT("AbilitySet에 나중에 추가된 Ability도 GiveAbility가 잘 진행되어야 함"),ASC->FindAbilitySpecFromClass(UMockGameplayAbility_InputTriggered::StaticClass()));
	
	const TArray<FActiveGameplayEffectHandle>& GEHandles = GrantedHandles.GetGameplayEffectHandles();
	
	TestNotNull(TEXT("AbilitySet에 등록된 GE는 ASC에서 Active된 상태여야함"),ASC->GetActiveGameplayEffect(GEHandles[0]));
	TestNotNull(TEXT("AbilitySet에 나중에 추가된 GE도 ASC에서 Active된 상태여야함"),ASC->GetActiveGameplayEffect(GEHandles[1]));
	
	TestNotNull(TEXT("AbilitySet에 등록된 AttributeSet은 ASC에 추가되어 있어야 함"), ASC->GetSet<UMockAttributeSet_A>());
	TestNotNull(TEXT("AbilitySet에 나중에 추가된 AttributeSet도 ASC에 추가되어 있어야 함"), ASC->GetSet<UMockAttributeSet_B>());
	
	
	return true;
}

bool FDamageExecutionTest::RunTest(const FString& Parameters)
{
	if (!SetupWorld())
	{
		return false;
	}
	
	ON_SCOPE_EXIT{
		ClearWorld();
	};
	
	AActor* SourceActor = GetWorld()->SpawnActor<AActor>();
	UAbilitySystemComponent* SourceASC = NewObject<UAbilitySystemComponent>(SourceActor);
	UBattleHealthSet* SourceHealthSet = NewObject<UBattleHealthSet>(SourceActor, TEXT("TestSourceHealthSet"));
	UBattleCombatSet* SourceCombatSet = NewObject<UBattleCombatSet>(SourceActor, TEXT("TestSourceCombatSet"));
	SourceASC->AddAttributeSetSubobject(SourceHealthSet);
	SourceASC->AddAttributeSetSubobject(SourceCombatSet);
	
	SourceActor->AddInstanceComponent(SourceASC);
	SourceASC->RegisterComponent();
	
	SourceASC->InitAbilityActorInfo(SourceActor,SourceActor);
	
	AActor* TargetActor = GetWorld()->SpawnActor<AActor>();
	UAbilitySystemComponent* TargetASC = NewObject<UAbilitySystemComponent>(TargetActor);
	UBattleHealthSet* TargetHealthSet = NewObject<UBattleHealthSet>(TargetActor, TEXT("TestTargetHealthSet"));
	UBattleCombatSet* TargetCombatSet = NewObject<UBattleCombatSet>(TargetActor, TEXT("TestTargetCombatSet"));
	UBattleEnemySet* TargetEnemySet = NewObject<UBattleEnemySet>(TargetActor, TEXT("TestTargetEnemySet"));
	TargetASC->AddAttributeSetSubobject(TargetHealthSet);
	TargetASC->AddAttributeSetSubobject(TargetCombatSet);
	TargetASC->AddAttributeSetSubobject(TargetEnemySet);
	
	TargetActor->AddInstanceComponent(TargetASC);
	TargetASC->RegisterComponent();
	
	TargetASC->InitAbilityActorInfo(TargetActor,TargetActor);
	
	const float CallerSourceBaseDamage = 50.f;
	const float CallerSourceAttackRate = 1.2f;
	const float CallerSourceImpactPower = 10.f;
	const float SourceAttackPower = 10.f;
	const float TargetImpactResistance = 5.f;
	const float TargetMaxHealth = 100.f;
	const float TargetHealth = 100.f;
	const float TargetMaxGroggy = 100.f;
	const float TargetGroggy = 0.0f;
	const float TargetMaxPoise = 100.f;
	const float TargetPoise = 100.f;
	
	SourceCombatSet->SetAttackPower(SourceAttackPower);
	TargetCombatSet->SetImpactResistance(TargetImpactResistance);
	TargetHealthSet->SetMaxHealth(TargetMaxHealth);
	TargetHealthSet->SetHealth(TargetHealth);
	TargetEnemySet->SetMaxGroggyValue(TargetMaxGroggy);
	TargetEnemySet->SetGroggyValue(TargetGroggy);
	TargetEnemySet->SetMaxPoiseValue(TargetMaxPoise);
	TargetEnemySet->SetPoiseValue(TargetPoise);
	
	const float TotalDamage = CallerSourceBaseDamage + SourceAttackPower * CallerSourceAttackRate;
	const float TotalImpactDamage = CallerSourceImpactPower - TargetImpactResistance;
	const float WantedHealth = TargetHealth - TotalDamage;
	const float WantedGroggy = TargetGroggy + TotalImpactDamage;
	const float WantedPoise = TargetPoise - TotalImpactDamage;
	bool bImpactDamageDelegate = false;
	
	TargetCombatSet->OnReceivedImpactDamage.AddLambda([&](AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude)
	{
		if (TotalImpactDamage == DamageMagnitude)
		{
			bImpactDamageDelegate = true;
		}
	});
	
	
	UGameplayEffect* DamageGE = NewObject<UGameplayEffect>(GetTransientPackage());
	DamageGE->DurationPolicy = EGameplayEffectDurationType::Instant;
	
	FGameplayEffectExecutionDefinition DamageExecution;
	DamageExecution.CalculationClass = UBattleDamageExecution::StaticClass();
	DamageGE->Executions.Add(DamageExecution);
	
	FGameplayEffectSpec Spec(DamageGE, SourceASC->MakeEffectContext(), 1.0f);
	Spec.SetSetByCallerMagnitude(FBattleGameplayTags::Get().GameplayEffect_Data_BaseDamage, CallerSourceBaseDamage);
	Spec.SetSetByCallerMagnitude(FBattleGameplayTags::Get().GameplayEffect_Data_AttackRate, CallerSourceAttackRate);
	Spec.SetSetByCallerMagnitude(FBattleGameplayTags::Get().GameplayEffect_Data_ImpactPower, CallerSourceImpactPower);
	
	SourceASC->ApplyGameplayEffectSpecToTarget(Spec, TargetASC);
	
	// Hp에 데미지가 제대로 적용되어야 함
	// Groggy, Poise에 ImpaceDamage가 잘 적용되어야 함
	// ImpaceDamage가 CombatSet에 적용되어야 함
	
	TestEqual(TEXT("Health에 Damage 공식이 잘 적용되어야 함"), TargetHealthSet->GetHealth(), WantedHealth);
	TestEqual(TEXT("Groggy 수치에 ImpactDamage가 잘 적용되어야 함"), TargetEnemySet->GetGroggyValue(), WantedGroggy);
	TestEqual(TEXT("Poise 수치에 ImpactDamage가 잘 적용되어야 함"), TargetEnemySet->GetPoiseValue(), WantedPoise);
	TestTrue(TEXT("ImpactDamage가 CombatSet에 제대로 적용되어야 함"), bImpactDamageDelegate);
	
	return true;
}

bool FHealthAttributeSetTest::RunTest(const FString& Parameters)
{
	if (!SetupWorld())
	{
		return false;
	}
	
	ON_SCOPE_EXIT{
		ClearWorld();
	};
	
	AActor* Actor = GetWorld()->SpawnActor<AActor>();
	UAbilitySystemComponent* ASC = NewObject<UAbilitySystemComponent>(Actor);
	UBattleHealthSet* HealthSet = NewObject<UBattleHealthSet>(Actor, TEXT("TestHealthSet"));
	ASC->AddAttributeSetSubobject(HealthSet);
	
	Actor->AddInstanceComponent(ASC);
	ASC->RegisterComponent();
	
	ASC->InitAbilityActorInfo(Actor,Actor);
	
	const float MaxHealthValue = 100.f;
	const float CurHealthValue = 20.f;
	const float MaxStaminaValue = 100.f;
	const float CurStaminaValue = 20.f;
	const float CalcHealthValue = CurHealthValue - CurHealthValue/2 + CurHealthValue*2; 
	
	HealthSet->SetMaxHealth(MaxHealthValue);
	HealthSet->SetHealth(CurHealthValue);
	HealthSet->SetMaxStamina(MaxStaminaValue);
	HealthSet->SetStamina(CurStaminaValue);
	
	UGameplayEffect* DamageGE = NewObject<UGameplayEffect>(GetTransientPackage());
	DamageGE->DurationPolicy = EGameplayEffectDurationType::Instant;
	FGameplayModifierInfo DamageModifier;
	DamageModifier.Attribute = UBattleHealthSet::GetDamageAttribute();
	DamageModifier.ModifierOp = EGameplayModOp::Additive;
	DamageModifier.ModifierMagnitude = FScalableFloat(CurHealthValue/2);
	DamageGE->Modifiers.Add(DamageModifier);
	
	FGameplayModifierInfo HealingModifier;
	HealingModifier.Attribute = UBattleHealthSet::GetHealingAttribute();
	HealingModifier.ModifierOp = EGameplayModOp::Additive;
	HealingModifier.ModifierMagnitude = FScalableFloat(CurHealthValue*2);
	DamageGE->Modifiers.Add(HealingModifier);
	
	ASC->ApplyGameplayEffectToSelf(DamageGE,1.0f, ASC->MakeEffectContext());
	
	TestEqual(TEXT("Healing은 메타 Attribute이기에 적용된 이후 0.0이 되어야 함"), HealthSet->GetHealing(), 0.0f);
	TestEqual(TEXT("Damage는 메타 Attribute이기에 적용된 이후 0.0이 되어야 함"), HealthSet->GetDamage(), 0.0f);
	TestEqual(TEXT("Healing과 Damage는 Health에 적용되어야 함"), HealthSet->GetHealth(), CalcHealthValue);
	
	
	bool bOutOfDeath = false;
	
	HealthSet->OnOutOfHealth.AddLambda([&](AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude)
	{
		bOutOfDeath = true;
	});
	
	UGameplayEffect* ClampTestGE = NewObject<UGameplayEffect>(GetTransientPackage());
	ClampTestGE->DurationPolicy = EGameplayEffectDurationType::Instant;
	FGameplayModifierInfo HealthModifier;
	HealthModifier.Attribute = UBattleHealthSet::GetHealthAttribute();
	HealthModifier.ModifierOp = EGameplayModOp::Additive;
	HealthModifier.ModifierMagnitude = FScalableFloat(-MaxHealthValue*2);
	ClampTestGE->Modifiers.Add(HealthModifier);
	
	FGameplayModifierInfo MaxHealthModifier;
	MaxHealthModifier.Attribute = UBattleHealthSet::GetMaxHealthAttribute();
	MaxHealthModifier.ModifierOp = EGameplayModOp::Additive;
	MaxHealthModifier.ModifierMagnitude = FScalableFloat(-MaxHealthValue*2);
	ClampTestGE->Modifiers.Add(MaxHealthModifier);
	
	FGameplayModifierInfo StaminaModifier;
	StaminaModifier.Attribute = UBattleHealthSet::GetStaminaAttribute();
	StaminaModifier.ModifierOp = EGameplayModOp::Additive;
	StaminaModifier.ModifierMagnitude = FScalableFloat(-MaxStaminaValue*2);
	ClampTestGE->Modifiers.Add(StaminaModifier);
	
	FGameplayModifierInfo MaxStaminaModifier;
	MaxStaminaModifier.Attribute = UBattleHealthSet::GetMaxStaminaAttribute();
	MaxStaminaModifier.ModifierOp = EGameplayModOp::Additive;
	MaxStaminaModifier.ModifierMagnitude = FScalableFloat(-MaxStaminaValue*2);
	ClampTestGE->Modifiers.Add(MaxStaminaModifier);
	
	ASC->ApplyGameplayEffectToSelf(ClampTestGE,1.0f, ASC->MakeEffectContext());
	
	TestEqual(TEXT("Health값은 음수가 될 수 없음."), HealthSet->GetHealth(), 0.0f);
	TestEqual(TEXT("Stamina값은 음수가 될 수 없음."), HealthSet->GetStamina(), 0.0f);
	TestEqual(TEXT("MaxHealth값은 1보다 작아질 수 없음."), HealthSet->GetMaxHealth(), 1.0f);
	TestEqual(TEXT("MaxStamina값은 1보다 작아질 수 없음."), HealthSet->GetMaxStamina(), 1.0f);
	
	TestTrue(TEXT("Health값이 0이하가 되면 죽음 델리게이트가 동작 해야함"), bOutOfDeath);
	
	
	UGameplayEffect* MaxClampTestGE = NewObject<UGameplayEffect>(GetTransientPackage());
	MaxClampTestGE->DurationPolicy = EGameplayEffectDurationType::Instant;
	HealthModifier.ModifierMagnitude = FScalableFloat(MaxHealthValue*2);
	StaminaModifier.ModifierMagnitude = FScalableFloat(MaxStaminaValue*2);
	
	MaxClampTestGE->Modifiers.Add(HealthModifier);
	MaxClampTestGE->Modifiers.Add(StaminaModifier);
	
	ASC->ApplyGameplayEffectToSelf(MaxClampTestGE,1.0f, ASC->MakeEffectContext());
	
	TestEqual(TEXT("Health값은 MaxHealth값보다 커질 수 없음."), HealthSet->GetHealth(), HealthSet->GetMaxHealth());
	TestEqual(TEXT("Stamina값은 MaxStamina값보다 커질 수 없음."), HealthSet->GetStamina(), HealthSet->GetMaxStamina());
	
	
	
	
	
	return true;
}

bool FEnemyAttributeSetTest::RunTest(const FString& Parameters)
{
	if (!SetupWorld())
	{
		return false;
	}
	
	ON_SCOPE_EXIT{
		ClearWorld();
	};
	
	
	AActor* Actor = GetWorld()->SpawnActor<AActor>();
	UAbilitySystemComponent* ASC = NewObject<UAbilitySystemComponent>(Actor);
	UBattleEnemySet* EnemySet = NewObject<UBattleEnemySet>(Actor, TEXT("TestEnemySet"));
	ASC->AddAttributeSetSubobject(EnemySet);
	
	Actor->AddInstanceComponent(ASC);
	ASC->RegisterComponent();
	
	ASC->InitAbilityActorInfo(Actor,Actor);
	
	const float MaxGroggy = 100.f;
	const float MaxPoise = 100.f;
	
	EnemySet->SetGroggyValue(0.0f);
	EnemySet->SetMaxGroggyValue(MaxGroggy);
	EnemySet->SetPoiseValue(50.f);
	EnemySet->SetMaxPoiseValue(MaxPoise);
	
	
	UGameplayEffect* DamageGE = NewObject<UGameplayEffect>(GetTransientPackage());
	DamageGE->DurationPolicy = EGameplayEffectDurationType::Instant;
	FGameplayModifierInfo GroggyModifier;
	GroggyModifier.Attribute = UBattleEnemySet::GetGroggyValueAttribute();
	GroggyModifier.ModifierOp = EGameplayModOp::Additive;
	GroggyModifier.ModifierMagnitude = FScalableFloat(MaxGroggy);
	DamageGE->Modifiers.Add(GroggyModifier);
	
	FGameplayModifierInfo PoiseModifier;
	PoiseModifier.Attribute = UBattleEnemySet::GetPoiseValueAttribute();
	PoiseModifier.ModifierOp = EGameplayModOp::Additive;
	PoiseModifier.ModifierMagnitude = FScalableFloat(-MaxPoise);
	DamageGE->Modifiers.Add(PoiseModifier);
	
	bool bOnGroggyDelegate = false;
	bool bOnPoiseDelegate = false;
	
	EnemySet->OnGroggyState.AddLambda([&](AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude)
	{
		bOnGroggyDelegate = true;
	});
	
	EnemySet->OnPoiseBreakState.AddLambda([&](AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude)
	{
		bOnPoiseDelegate = true;
	});
	
	ASC->ApplyGameplayEffectToSelf(DamageGE,1.0f, ASC->MakeEffectContext());
	
	TestEqual(TEXT("Groggy수치가 MaxGroggy이상이 되면 다시 0.0으로 돌아와야 함"), EnemySet->GetGroggyValue(), 0.0f);
	TestEqual(TEXT("Poise수치가 0.0이하가 되면 MaxPoise값으로 돌아와야 함"), EnemySet->GetPoiseValue(), MaxPoise);
	
	TestTrue(TEXT("Groggy 수치가 MaxGroggy이상이 되면 OnGroggy Delegate가 동작해야함"), bOnGroggyDelegate);
	TestTrue(TEXT("Poise 수치가 0.0이하가 되면 OnPoiseBreak Delegate가 동작해야함"), bOnPoiseDelegate);
	
	UGameplayEffect* ClampTestGE = NewObject<UGameplayEffect>(GetTransientPackage());
	ClampTestGE->DurationPolicy = EGameplayEffectDurationType::Instant;
	GroggyModifier.Attribute = UBattleEnemySet::GetGroggyValueAttribute();
	GroggyModifier.ModifierOp = EGameplayModOp::Additive;
	GroggyModifier.ModifierMagnitude = FScalableFloat(-MaxGroggy*2);
	ClampTestGE->Modifiers.Add(GroggyModifier);
	
	PoiseModifier.Attribute = UBattleEnemySet::GetPoiseValueAttribute();
	PoiseModifier.ModifierOp = EGameplayModOp::Additive;
	PoiseModifier.ModifierMagnitude = FScalableFloat(MaxPoise*2);
	ClampTestGE->Modifiers.Add(PoiseModifier);
	
	FGameplayModifierInfo MaxPoiseModifier;
	MaxPoiseModifier.Attribute = UBattleEnemySet::GetMaxPoiseValueAttribute();
	MaxPoiseModifier.ModifierOp = EGameplayModOp::Additive;
	MaxPoiseModifier.ModifierMagnitude = FScalableFloat(-MaxPoise*2);
	ClampTestGE->Modifiers.Add(MaxPoiseModifier);
	
	FGameplayModifierInfo MaxGroggyModifier;
	MaxGroggyModifier.Attribute = UBattleEnemySet::GetMaxGroggyValueAttribute();
	MaxGroggyModifier.ModifierOp = EGameplayModOp::Additive;
	MaxGroggyModifier.ModifierMagnitude = FScalableFloat(-MaxGroggy*2);
	ClampTestGE->Modifiers.Add(MaxGroggyModifier);
	
	ASC->ApplyGameplayEffectToSelf(ClampTestGE, 1.0f, ASC->MakeEffectContext());
	
	TestEqual(TEXT("Groggy값은 음수가 될 수 없습니다"), EnemySet->GetGroggyValue(), 0.0f);
	TestEqual(TEXT("Poise값은 MaxPoise보다 커질 수 없습니다"), EnemySet->GetPoiseValue(), EnemySet->GetMaxPoiseValue());
	TestEqual(TEXT("MaxGroggy값은 1보다 작아질 수 없습니다"), EnemySet->GetMaxGroggyValue(), 1.0f);
	TestEqual(TEXT("MaxPoise값은 1보다 작아질 수 없습니다"), EnemySet->GetMaxPoiseValue(), 1.0f);
	
	
	
	
	return true;
}

bool FCombatAttributeSetTest::RunTest(const FString& Parameters)
{
	if (!SetupWorld())
	{
		return false;
	}
	
	ON_SCOPE_EXIT{
		ClearWorld();
	};
	
	AActor* Actor = GetWorld()->SpawnActor<AActor>();
	UAbilitySystemComponent* ASC = NewObject<UAbilitySystemComponent>(Actor);
	
	UBattleCombatSet* CombatSet = NewObject<UBattleCombatSet>(Actor, TEXT("TestCombatSet"));
	ASC->AddAttributeSetSubobject(CombatSet);
	
	Actor->AddInstanceComponent(ASC);
	ASC->RegisterComponent();
	
	ASC->InitAbilityActorInfo(Actor,Actor);
	
	bool bCombatDelegate = false;
	const float ImpactDamage = 30.f;
	
	CombatSet->OnReceivedImpactDamage.AddLambda([&](AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude)
	{
		if (ImpactDamage == DamageMagnitude)
		{
			bCombatDelegate = true;	
		}
	});
	
	UGameplayEffect* DamageGE = NewObject<UGameplayEffect>(GetTransientPackage());
	DamageGE->DurationPolicy = EGameplayEffectDurationType::Instant;
	FGameplayModifierInfo Modifier;
	Modifier.Attribute = UBattleCombatSet::GetImpactDamageAttribute();
	Modifier.ModifierOp = EGameplayModOp::Additive;
	Modifier.ModifierMagnitude = FScalableFloat(ImpactDamage);
	DamageGE->Modifiers.Add(Modifier);
	
	ASC->ApplyGameplayEffectToSelf(DamageGE,1.0f, ASC->MakeEffectContext());
	
	TestEqual(TEXT("ImpactDamage는 메타 Attribute이기에 적용이 끝나면 0이 되어야 함"), CombatSet->GetImpactDamage(), 0.0f);
	
	TestTrue(TEXT("ImpactDamage가 발생하면 Delegate에 정확하게 전달되어야 함"), bCombatDelegate);
	return true;
}


bool FGameplayAbility_DeathTest_WaitEvent::Update()
{
	if (HealthComponent->IsDeadOrDying())
	{
		return true;
	}
	if (FPlatformTime::Seconds() >= StartTime +10)
	{
		Test->AddError(TEXT("TimeOver: Dead 상태가 되지 않음"));
		return true;
	}
	return false;
}

bool FGameplayAbility_DeathTest::RunTest(const FString& Parameters)
{
	if (!SetupWorld())
	{
		return false;
	}
	
	ON_SCOPE_EXIT{
		ClearWorld();
	};
	
	ABattleCharacter* PlayerCharacter = GetWorld()->SpawnActor<ABattleCharacter>();
	ABattlePlayerState* PlayerState = GetWorld()->SpawnActor<ABattlePlayerState>();
	ABattlePlayerController* PlayerController = GetWorld()->SpawnActor<ABattlePlayerController>();
	UAbilitySystemComponent* ASC = PlayerState->GetAbilitySystemComponent();
	
	UBattleHealthSet* HealthSet = NewObject<UBattleHealthSet>(PlayerState, TEXT("TestHealthSet"));
	ASC->AddAttributeSetSubobject(HealthSet);
	
	PlayerController->PlayerState = PlayerState;
	PlayerController->Possess(PlayerCharacter);
	PlayerCharacter->SetPlayerState(PlayerState);
	
	UBattlePawnExtensionComponent* PawnExtensionComponent = Cast<UBattlePawnExtensionComponent>(PlayerCharacter->GetComponentByClass(UBattlePawnExtensionComponent::StaticClass()));
	
	if (PawnExtensionComponent == nullptr)
	{
		return false;
	}
	
	PawnExtensionComponent->InitializeAbilitySystem(PlayerState->GetBattleAbilitySystemComponent(), PlayerState);
	
	BeginPlay();
	
	FGameplayAbilitySpec AbilitySpec(UBattleGameplayAbility_Death::StaticClass());
	FGameplayAbilitySpecHandle SpecHandle = ASC->GiveAbility(AbilitySpec);
	
	UBattleHealthComponent* HealthComponent = PlayerCharacter->GetHealthComponent();
	TestEqual(TEXT("Death를 하기 전에는 NotDead 상태여야 함"),HealthComponent->GetDeathState(), EBattleDeathState::NotDead);
	
	FGameplayEventData Payload;
	Payload.EventTag = FBattleGameplayTags::Get().GameplayEvent_Death;

	ASC->HandleGameplayEvent(Payload.EventTag, &Payload);
	
	ADD_LATENT_AUTOMATION_COMMAND(FGameplayAbility_DeathTest_WaitEvent(this, FPlatformTime::Seconds(), HealthComponent));
	
	
	return true;
}


bool FGameplayAbility_ConvertAITest::RunTest(const FString& Parameters)
{
	if (!SetupWorld())
	{
		return false;
	}
	
	ON_SCOPE_EXIT{
		ClearWorld();
	};
	
	ABattleCharacter* PlayerCharacter = GetWorld()->SpawnActor<ABattleCharacter>();
	ABattlePlayerState* PlayerState = GetWorld()->SpawnActor<ABattlePlayerState>();
	ABattlePlayerController* PlayerController = GetWorld()->SpawnActor<ABattlePlayerController>();
	UAbilitySystemComponent* ASC = PlayerState->GetAbilitySystemComponent();
	
	UBattleHealthSet* HealthSet = NewObject<UBattleHealthSet>(PlayerState, TEXT("TestHealthSet"));
	ASC->AddAttributeSetSubobject(HealthSet);
	
	PlayerController->PlayerState = PlayerState;
	PlayerController->Possess(PlayerCharacter);
	PlayerCharacter->SetPlayerState(PlayerState);
	
	UBattlePawnData* PawnData = NewObject<UBattlePawnData>(PlayerCharacter);
	
	UBattlePawnExtensionComponent* PawnExtensionComponent = Cast<UBattlePawnExtensionComponent>(PlayerCharacter->GetComponentByClass(UBattlePawnExtensionComponent::StaticClass()));
	
	if (PawnExtensionComponent == nullptr)
	{
		return false;
	}
	
	PawnExtensionComponent->SetPawnData(PawnData);
	PawnExtensionComponent->InitializeAbilitySystem(PlayerState->GetBattleAbilitySystemComponent(), PlayerState);
	
	
	BeginPlay();
	


	FGameplayAbilitySpec AbilitySpec(UBattleGameplayAbility_ConvertToAI::StaticClass());
	FGameplayAbilitySpecHandle SpecHandle = ASC->GiveAbility(AbilitySpec);
	ASC->TryActivateAbility(SpecHandle);
	
	TestNull(TEXT("PawnData에 제대로 된 정보가 없으면 ObserverPawn이 생성 안됨"), PlayerCharacter->GetObserverPawn());
	
	PawnData->ObserverInputData = NewObject<UBattleInputConfig>();
	TArray<FBattleMappableConfigPair> TestInputConfigs;
	TestInputConfigs.Add(FBattleMappableConfigPair());
	PawnData->ObserverInputConfigs = TestInputConfigs;
	PawnData->ObserverDefaultCameraMode = UBattleCameraMode::StaticClass();
	PawnData->AIComboGA = UBattleGameplayAbility_ComboAttack::StaticClass();
	PawnData->AIComboStrongGA = UBattleGameplayAbility_ComboStrongAttack::StaticClass();
	
	ASC->TryActivateAbility(SpecHandle);
	TestNotNull(TEXT("PawnData를 제대로 설정하면 ObserverPawn이 생성 되어야 함"), PlayerCharacter->GetObserverPawn());
	TestNotNull(TEXT("PawnData를 제대로 설정하면 AIController가 생성되어야 함"), PlayerCharacter->GetAiController());
	TestTrue(TEXT("ConvertToAI가 성공하면 AIController가 캐릭터를 컨트롤 해야함"), PlayerCharacter->GetController() == PlayerCharacter->GetAiController());
	TestTrue(TEXT("ConvertToAI가 성공하면 기존 컨트롤러는 ObserverPawn에 Possess되어야 함"), PlayerCharacter->GetObserverPawn()->GetController() == PlayerController);

	ASC->CancelAbilityHandle(SpecHandle);
	
	PawnExtensionComponent->SetPawnData(nullptr);
	PlayerController->Possess(PlayerCharacter);
	
	ASC->TryActivateAbility(SpecHandle);
	TestTrue(TEXT("첫 생성 이후 PawnData가 없어도 AIController가 캐릭터를 컨트롤 해야함"), PlayerCharacter->GetController() == PlayerCharacter->GetAiController());
	TestTrue(TEXT("첫 생성 이후 PawnData가 없어도 ObserverPawn에 Possess되어야 함"), PlayerCharacter->GetObserverPawn()->GetController() == PlayerController);
	
	return true;
}


UMockTestGameplayAbilityBase::UMockTestGameplayAbilityBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	
}

void UMockTestGameplayAbilityBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                   const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                   const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	ActivateCount++;
}

void UMockTestGameplayAbilityBase::InputPressed(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
	InputPressedCount++;
}

void UMockTestGameplayAbilityBase::InputReleased(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	InputReleasedCount++;
}

void UMockTestGameplayAbilityBase::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	EndAbilityCount++;
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

UMockTestGameplayAbility_A::UMockTestGameplayAbility_A(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilityTags.AddTag(FGATag_A);
}

UMockTestGameplayAbility_B::UMockTestGameplayAbility_B(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilityTags.AddTag(FGATag_B);
}

UMockTestGameplayAbility_C::UMockTestGameplayAbility_C(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilityTags.AddTag(FGATag_C);
}

UMockGameplayAbility_WhileInputActive::UMockGameplayAbility_WhileInputActive(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ActivationPolicy = EBattleAbilityActivationPolicy::WhileInputActive;
}

UMockGameplayAbility_InputTriggered::UMockGameplayAbility_InputTriggered(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ActivationPolicy = EBattleAbilityActivationPolicy::OnInputTriggered;
}

UMockGameplayEffect_A::UMockGameplayEffect_A(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;
}

UMockGameplayEffect_B::UMockGameplayEffect_B(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;
}

UMockAttributeSet_A::UMockAttributeSet_A(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

UMockAttributeSet_B::UMockAttributeSet_B(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

UMockGamePhaseAbility_A::UMockGamePhaseAbility_A(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GamePhaseTag = FPhaseTag_A;
}

UMockGamePhaseAbility_B::UMockGamePhaseAbility_B(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	GamePhaseTag = FPhaseTag_B;
}

UMockGamePhaseAbility_A_C::UMockGamePhaseAbility_A_C(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	GamePhaseTag = FPhaseTag_A_C;
}


bool FPhaseSystemTest::RunTest(const FString& Parameters)
{
	if (!SetupWorld())
	{
		return false;
	}
	
	ON_SCOPE_EXIT{
		ClearWorld();
	};
	
	UBattleGamePhaseSubsystem* BattleGamePhaseSubsystem = Cast<UBattleGamePhaseSubsystem>(GetWorld()->GetSubsystem<UBattleGamePhaseSubsystem>());
	ABattleGameState* BattleGameState = GetWorld()->SpawnActor<ABattleGameState>();
	GetWorld()->SetGameState(BattleGameState);
	InitializeActors();
	
	TestNotNull(TEXT("World에 PhaseSystem이 존재해야함"), BattleGamePhaseSubsystem);
	TestNotNull(TEXT("GameState 존재"), GetWorld()->GetGameState());
	TestNotNull(TEXT("BattleGameState 존재"), BattleGameState);
	
	
	BattleGamePhaseSubsystem->StartPhase(UMockGamePhaseAbility_A::StaticClass());
	TestTrue(TEXT("Phase가 Active되어야 함"),BattleGamePhaseSubsystem->IsPhaseActive(FPhaseTag_A));
	
	bool PhaseObserverBound = false;
	
	FBattleGamePhaseTagDelegate PhaseActiveDelegate;
	
	PhaseActiveDelegate.BindLambda([&](const FGameplayTag& PhaseTag)
	{
		PhaseObserverBound = true;
	});
	
	BattleGamePhaseSubsystem->WhenPhaseStartsOrIsActive(FPhaseTag_A_C, EPhaseTagMatchType::ExactMatch, PhaseActiveDelegate);
	
	BattleGamePhaseSubsystem->StartPhase(UMockGamePhaseAbility_A_C::StaticClass());
	TestTrue(TEXT("자식 태그가 들어오면 부모 페이즈는 유지되어야 함"), BattleGamePhaseSubsystem->IsPhaseActive(FPhaseTag_A));
	TestTrue(TEXT("페이즈 시작 바인딩을 걸었으면, 페이즈 시작시 실행되어야 함"), PhaseObserverBound);
	
	BattleGamePhaseSubsystem->StartPhase(UMockGamePhaseAbility_B::StaticClass());
	TestFalse(TEXT("관련 없는 태그가 들어오면 이전 페이즈는 취소 되어야 함"), BattleGamePhaseSubsystem->IsPhaseActive(FPhaseTag_A_C));
	
	return true;
	
	
}


#endif

