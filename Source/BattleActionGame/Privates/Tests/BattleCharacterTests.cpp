

#if WITH_DEV_AUTOMATION_TESTS

#include "BattleCharacterTests.h"
#include "GameplayEffect.h"
#include "BattleActorTestFixture.h"
#include "BattleActionGame/AbilitySystem/BattleAbilitySystemComponent.h"
#include "BattleActionGame/AbilitySystem/Attributes/BattleHealthSet.h"
#include "BattleActionGame/Character/BattleCharacterBase.h"
#include "BattleActionGame/Character/BattleHealthComponent.h"
#include "BattleActionGame/Character/BattleHeroComponent.h"
#include "BattleActionGame/Character/BattlePawnExtensionComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(FCharacterStopMotionTest, FBattleActorTestFixture, "BattleAction.Character.StopMotion", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter);
IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(FBattlePlayerCharacterTest, FBattleActorTestFixture, "BattleAction.Character.PlayerCharacter", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter);
IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(FHealthComponentTest, FBattleActorTestFixture, "BattleAction.Character.HealthComponent", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter);
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FHeroComponentTest, "BattleAction.Character.HeroComponent", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter);
IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(FPawnExtComponentTest, FBattleActorTestFixture, "BattleAction.Character.PawnExtensionComponent", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter);

bool FPawnExtComponentTest::RunTest(const FString& Parameters)
{
	if (!SetupWorld())
	{
		return false;
	}
	
	ON_SCOPE_EXIT{
		ClearWorld();
	};
	
	APawn* TestActor = GetWorld()->SpawnActor<APawn>();
	AActor* OwnerActor = GetWorld()->SpawnActor<AActor>();
	UBattleAbilitySystemComponent* ASC = NewObject<UBattleAbilitySystemComponent>(TestActor);
	TestActor->AddInstanceComponent(ASC);
	ASC->RegisterComponent();
	UBattlePawnExtensionComponent* PawnExtComp = NewObject<UBattlePawnExtensionComponent>(TestActor);
	TestActor->AddInstanceComponent(PawnExtComp);
	PawnExtComp->RegisterComponent();
	
	bool bSuccessInitASC = false;
	
	PawnExtComp->OnAbilitySystemInitialized.AddLambda([&bSuccessInitASC]()
	{
		bSuccessInitASC = true;
	});
	
	PawnExtComp->InitializeAbilitySystem(ASC, OwnerActor);
	
	TestTrue(TEXT("PawnExtComp가 InitASC를 하고 난 후 Delegate가 Broadcast되어야 함"), bSuccessInitASC);
	TestEqual(TEXT("InitASC가 된 이후 해당 ASC를 캐시해서 가지고 있어야 함"), PawnExtComp->GetAbilitySystemComponent(), ASC);
	TestTrue(TEXT("AvatarActor 설정 체크"), ASC->GetAvatarActor() == TestActor);
	TestEqual(TEXT("OwnerActor 설정 체크"), ASC->GetOwnerActor(), OwnerActor);
	
	bool bSuccessUninitASC = false;
	PawnExtComp->OnAbilitySystemUninitialized.AddLambda([&bSuccessUninitASC]()
	{
		bSuccessUninitASC = true;
	});
	
	PawnExtComp->UnInitializeAbilitySystem();
	
	TestTrue(TEXT("PawnExtComp가 UninitASC를 하고 난 후 Delegate가 Broadcast되어야 함"), bSuccessUninitASC);
	TestNull(TEXT("UninitASC 이후 캐시된 ASC는 null이어야 함"), PawnExtComp->GetAbilitySystemComponent());
	TestNull(TEXT("AvatarActor 초기화 체크"), ASC->GetAvatarActor());
	
	return true;
}

bool FHeroComponentTest::RunTest(const FString& Parameters)
{
	UBattleHeroComponent* HeroComponent = NewObject<UBattleHeroComponent>();
	
	FVector WantedVector = FVector(1000.f,2000.f, 500.f);
	FVector TestDir = FVector(1.f,2.f,0.0f);
	float Strength = 1000.f;
	float ZForce = 500.f;
	
	FVector CalcVector;
	
	HeroComponent->CalcKnockbackVector(TestDir, Strength, ZForce, CalcVector);
	
	TestEqual(TEXT("Knockback 계산식에 맞는 결과 Vector가 나와야 함"), CalcVector, WantedVector);
	
	
	return true;
}

bool FHealthComponentTest::RunTest(const FString& Parameters)
{
	if (!SetupWorld())
	{
		return false;
	}
	
	ON_SCOPE_EXIT{
		ClearWorld();
	};
	
	AActor* TestActor = GetWorld()->SpawnActor<AActor>();
	UBattleAbilitySystemComponent* ASC = NewObject<UBattleAbilitySystemComponent>(TestActor);
	UBattleHealthComponent* HealthComponent = NewObject<UBattleHealthComponent>(TestActor);
	UBattleHealthSet* HealthSet = NewObject<UBattleHealthSet>(TestActor);
	ASC->AddAttributeSetSubobject(HealthSet);
	
	TestActor->AddInstanceComponent(ASC);
	TestActor->AddInstanceComponent(HealthComponent);
	
	ASC->RegisterComponent();
	HealthComponent->RegisterComponent();
	
	ASC->InitAbilityActorInfo(TestActor, TestActor);
	HealthComponent->InitializeWithAbilitySystem(ASC);
	
	UMockTestObject* MockTestObject = NewObject<UMockTestObject>();
	
	bool bTest = false;
	
	
	HealthComponent->OnMaxHealthChanged.AddDynamic(MockTestObject, &UMockTestObject::HealthAttributeChange);
	HealthSet->SetMaxHealth(100.f);
	HealthComponent->OnMaxHealthChanged.Remove(MockTestObject,FName("HealthAttributeChange"));
	
	TestEqual(TEXT("MaxHealth가 변경되면, OnMaxHealthChanged Delegate가 Broadcast되어야 함"), MockTestObject->HealthAttributeChangeCount, 1);
	
	HealthComponent->OnHealthChanged.AddDynamic(MockTestObject, &UMockTestObject::HealthAttributeChange);
	HealthSet->SetHealth(50.f);
	HealthComponent->OnHealthChanged.Remove(MockTestObject,FName("HealthAttributeChange"));
	
	TestEqual(TEXT("Health가 변경되면, OnHealthChanged Delegate가 Broadcast되어야 함"), MockTestObject->HealthAttributeChangeCount, 2);
	
	HealthComponent->OnMaxStaminaChanged.AddDynamic(MockTestObject, &UMockTestObject::HealthAttributeChange);
	HealthSet->SetMaxStamina(100.f);
	HealthComponent->OnMaxStaminaChanged.Remove(MockTestObject,FName("HealthAttributeChange"));
	
	TestEqual(TEXT("MaxStamina가 변경되면, OnMaxStaminaChanged Delegate가 Broadcast되어야 함"), MockTestObject->HealthAttributeChangeCount, 3);
	
	HealthComponent->OnStaminaChanged.AddDynamic(MockTestObject, &UMockTestObject::HealthAttributeChange);
	HealthSet->SetStamina(50.f);
	HealthComponent->OnStaminaChanged.Remove(MockTestObject,FName("HealthAttributeChange"));
	
	TestEqual(TEXT("Stamina가 변경되면, OnStaminaChanged Delegate가 Broadcast되어야 함"), MockTestObject->HealthAttributeChangeCount, 4);
	
	
	TestEqual(TEXT("HealthNormalized는 제대로 된 값을 리턴 해야함"),HealthComponent->GetHealthNormalized(), 0.5f);
	
	TestTrue(TEXT("Death를 진행하기 전에는 NotDead 상태여야함"), HealthComponent->GetDeathState() == EBattleDeathState::NotDead);
	
	
	HealthComponent->OnDeathStarted.AddDynamic(MockTestObject, &UMockTestObject::ActorEvent);
	HealthComponent->StartDeath();
	HealthComponent->OnDeathStarted.Remove(MockTestObject, FName("ActorEvent"));
	
	TestTrue(TEXT("Death가 시작되면 DeathStarted로 바뀌어야 함"), HealthComponent->GetDeathState() == EBattleDeathState::DeathStarted);
	
	
	HealthComponent->OnDeathFinished.AddDynamic(MockTestObject, &UMockTestObject::ActorEvent);
	HealthComponent->FinishDeath();
	HealthComponent->OnDeathFinished.Remove(MockTestObject, FName("ActorEvent"));
	
	
	TestTrue(TEXT("Death가 끝나면 DeathFinished로 바뀌어야 함"), HealthComponent->GetDeathState() == EBattleDeathState::DeathFinished);
	TestEqual(TEXT("Death가 끝나면 OnDeathFinished Delegate가 Broadcast되어야 함"), MockTestObject->ActorEventCount, 2);
	
	
	
	
	
	
	
	
	
	
	return true;
}


bool FBattlePlayerCharacterTest::RunTest(const FString& Parameters)
{
	if (!SetupWorld())
	{
		return false;
	}
	
	ON_SCOPE_EXIT{
		ClearWorld();
	};
	
	AMockTestBattleCharacter* Character = GetWorld()->SpawnActor<AMockTestBattleCharacter>();
	
	float GroggyThreshold = Character->GetGroggyThreshold();
	float PoiseBreakThreshold = Character->GetPoiseBreakThreshold();
	Character->TestHandleImpactDamage(PoiseBreakThreshold-1);
	
	TestEqual(TEXT("PoiseBreakThreshold "), Character->PoiseBreakCount, 0);
	TestEqual(TEXT("PoiseBreakThreshold "), Character->GroggyCount, 0);
	
	Character->TestHandleImpactDamage(PoiseBreakThreshold+1);
	
	TestEqual(TEXT("PoiseBreakThreshold "), Character->PoiseBreakCount, 1);
	TestEqual(TEXT("PoiseBreakThreshold "), Character->GroggyCount, 0);
	
	Character->TestHandleImpactDamage(GroggyThreshold+1);
	
	TestEqual(TEXT("PoiseBreakThreshold "), Character->PoiseBreakCount, 1);
	TestEqual(TEXT("PoiseBreakThreshold "), Character->GroggyCount, 1);
	
	
	Character->DisableMovementAndCollision();
	UCapsuleComponent* CapsuleComponent = Character->GetCapsuleComponent();
	UCharacterMovementComponent* MovementComponent = Character->GetCharacterMovement();
	
	TestTrue(TEXT(""),CapsuleComponent->GetCollisionEnabled() == ECollisionEnabled::NoCollision);
	TestTrue(TEXT(""), MovementComponent->MovementMode == EMovementMode::MOVE_None);
	
	
	return true;
	
}


bool FCharacterStopMotionTest::RunTest(const FString& Parameters)
{
	if (!SetupWorld())
	{
		return false;
	}
	
	ON_SCOPE_EXIT{
		ClearWorld();
	};
	
	ABattleCharacterBase* Character = GetWorld()->SpawnActor<ABattleCharacterBase>();
	
	Character->MulticastStopMotion_Implementation(0.1f, 0.01f);
	
	TestEqual(TEXT("StopMotion이 호출되면 TimeDilation이 정해진만큼 적용되어야 함"), Character->CustomTimeDilation, 0.01f);
	
	GFrameCounter++;
	Character->GetWorldTimerManager().Tick(0.1f);
	GFrameCounter++;
	Character->GetWorldTimerManager().Tick(1.0f);
	
	TestEqual(TEXT("StopMotion이 호출되면 TimeDilation이 정해진만큼 적용되어야 함"), Character->CustomTimeDilation, 1.f);
	
	
	return true;
}


#endif

void AMockTestBattleCharacter::OnGroggy()
{
	Super::OnGroggy();
	GroggyCount++;
}

void AMockTestBattleCharacter::OnPoiseBreak()
{
	Super::OnPoiseBreak();
	PoiseBreakCount++;
}

void AMockTestBattleCharacter::TestHandleImpactDamage(float DamageMagnitude)
{
	FGameplayEffectSpec Spec;
	HandleImpactDamage(nullptr, nullptr, Spec, DamageMagnitude);
}

void UMockTestBattleCombatManagerComponent::OnHitEvent(const FBattleHitMessage& HitMessage)
{
	HitEventCount++;
	Super::OnHitEvent(HitMessage);
}
