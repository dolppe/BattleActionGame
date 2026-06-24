
#if WITH_DEV_AUTOMATION_TESTS
#include "CameraTests.h"

#include "BattleActorTestFixture.h"
#include "BattleActionGame/Camera/BattleCameraComponent.h"
#include "BattleActionGame/Camera/BattleCameraMode.h"

IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(FCameraTest, FBattleActorTestFixture, "BattleAction.Camera", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter);

bool FCameraTest::RunTest(const FString& Parameters)
{
	if (!SetupWorld())
	{
		return false;
	}
	
	ON_SCOPE_EXIT{
		ClearWorld();
	};
	
	AActor* TestActor = GetWorld()->SpawnActor<AActor>();
	UBattleCameraComponent* CameraComponent = NewObject<UBattleCameraComponent>(TestActor);
	
	TestActor->AddInstanceComponent(CameraComponent);
	CameraComponent->RegisterComponent();
	
	TestNotNull(TEXT("카메라 Component가 Register되면 CameraModeStack이 생성되어야 함"),CameraComponent->CameraModeStack.Get());
	
	if (CameraComponent->CameraModeStack.IsNull())
	{
		return false;
	}
	
	CameraComponent->DetermineCameraModeDelegate.BindLambda([]()
	{
		return UMockTestCameraMode_A::StaticClass();
	});
	
	CameraComponent->UpdateCameraModes();
	TestEqual(TEXT("DetermineCameraModeDelegate가 걸린 상태로 CameraMode를 업데이트 하면 Stack에 Mode가 Push되어야 함"),CameraComponent->CameraModeStack->CameraModeStack.Num(), 1);
	
	if (CameraComponent->CameraModeStack->CameraModeStack.IsEmpty())
	{
		return false;
	}
	
	TestEqual(TEXT("CameraStack의 Bottom은 항상 BlendWeight이 1.0f이어야 함"), CameraComponent->CameraModeStack->CameraModeStack[0]->GetBlendWeight(), 1.0f);
	
	FBattleCameraModeView CurCameraView;
	CameraComponent->CameraModeStack->EvaluateStack(0.2f, CurCameraView);
	
	TestEqual(TEXT(""), CurCameraView.Location, FVector(10.f));
	
	CameraComponent->DetermineCameraModeDelegate.BindLambda([]()
	{
		return UMockTestCameraMode_B::StaticClass();
	});

	TSubclassOf<UBattleCameraMode> CameraModeClass_B = UMockTestCameraMode_B::StaticClass();
	
	CameraComponent->UpdateCameraModes();
	TestEqual(TEXT("다른 CameraMode를 등록한 후에 Update하고, 이게 Blend가 필요하다면, 해당 CameraMode는 Stack에 추가됨"), CameraComponent->CameraModeStack->CameraModeStack.Num(), 2);
	TestTrue(TEXT("신규 CameraMode는 Top(0)에 위치 해야함"),CameraComponent->CameraModeStack->CameraModeStack[0] == CameraComponent->CameraModeStack->GetCameraModeInstance(CameraModeClass_B));
	TestTrue(TEXT("Stack에 혼자가 아니고, BlendTime이 존재하는 CameraMode는 추가되면 BlendWeight이 0.0f여야 함"),CameraComponent->CameraModeStack->CameraModeStack[0]->GetBlendWeight() == 0.0f);
	
	CameraComponent->CameraModeStack->EvaluateStack(0.5f, CurCameraView);
	
	const float WantedCameraLocationX = (10.f + 100.f)*0.5f;
	
	TestTrue(TEXT("새로 추가된 CameraMode의 BlendTime에 따라 BlendAlpha가 진행되어 결과에 영향을 줘야함"), CurCameraView.Location.X == WantedCameraLocationX);
	
	
	CameraComponent->DetermineCameraModeDelegate.BindLambda([]()
	{
		return UMockTestCameraMode_A::StaticClass();
	});
	
	CameraComponent->UpdateCameraModes();
	
	CameraComponent->CameraModeStack->EvaluateStack(0.5f, CurCameraView);
	
	TestEqual(TEXT("A => B 0.5만큼 이동, 이후 다시 B => A로 0.5만큼 이동이기에 다시 ACameraMode가 되어야 함"),CurCameraView.Location, FVector(10.f));
	
	
	return true;
}





#endif

