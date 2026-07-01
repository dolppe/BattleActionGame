
#if WITH_DEV_AUTOMATION_TESTS
#include "BattleActorTestFixture.h"
#include "BattleActionGame/Character/BattleCharacter.h"
#include "BattleActionGame/Navigation/WorldRiskGridMap.h"
#include "Components/CapsuleComponent.h"

IMPLEMENT_CUSTOM_SIMPLE_AUTOMATION_TEST(FWorldRiskGridMapTest, FBattleActorTestFixture, "BattleAction.NavSystem.WorldRiskGridMap", EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter);

bool FWorldRiskGridMapTest::RunTest(const FString& Parameters)
{
	UWorld::InitializationValues NewIVS = UWorld::InitializationValues();
	NewIVS.bCreateNavigation = true;
	NewIVS.bCreatePhysicsScene = true;
	SetInitializationValues(&NewIVS);
	
	if (!SetupWorld())
	{
		return false;
	}
	
	ON_SCOPE_EXIT{
		ClearWorld();
	};
	
	UWorldRiskGridMapSubSystem* WorldRiskGridMapSubSystem = GetWorld()->GetSubsystem<UWorldRiskGridMapSubSystem>();
	
	FGridMapInitData TestInitData;
	TestInitData.RiskOffsetTable.Add(FRiskCellOffset(1,0,0.7f));
	TestInitData.RiskOffsetTable.Add(FRiskCellOffset(0,1,0.7f));
	TestInitData.RiskOffsetTable.Add(FRiskCellOffset(-1,0,0.7f));
	TestInitData.RiskOffsetTable.Add(FRiskCellOffset(0,-1,0.7f));
	
	TestInitData.RiskOffsetTable.Add(FRiskCellOffset(1,-1,0.5f));
	TestInitData.RiskOffsetTable.Add(FRiskCellOffset(-1,-1,0.5f));
	TestInitData.RiskOffsetTable.Add(FRiskCellOffset(1,1,0.5f));
	TestInitData.RiskOffsetTable.Add(FRiskCellOffset(-1,1,0.5f));
	
	TestInitData.RiskOffsetTable.Add(FRiskCellOffset(2,0,0.3f));
	TestInitData.RiskOffsetTable.Add(FRiskCellOffset(-2,0,0.3f));
	TestInitData.RiskOffsetTable.Add(FRiskCellOffset(0,2,0.3f));
	TestInitData.RiskOffsetTable.Add(FRiskCellOffset(0,-2,0.3f));
	
	TestInitData.StartLocation = FVector(0.0f,0.0f,0.0f);
	TestInitData.ChunkSize = 1000.f;
	TestInitData.NumChunkCols = 6;
	TestInitData.NumChunkRows = 6;
	TestInitData.CellSize = 100.f;
	
	WorldRiskGridMapSubSystem->Init(TestInitData);
	
	ABattleCharacter* TestCharacter_A = GetWorld()->SpawnActor<ABattleCharacter>();
	TestCharacter_A->GetCapsuleComponent()->SetCollisionProfileName(TEXT("Friend"));
	TestCharacter_A->GetCapsuleComponent()->SetCapsuleSize(1.f,10.f);
	TestCharacter_A->SetActorLocation(FVector(310.f,110.f,0.0f));
	
	ABattleCharacter* TestCharacter_B = GetWorld()->SpawnActor<ABattleCharacter>();
	TestCharacter_B->GetCapsuleComponent()->SetCollisionProfileName(TEXT("Friend"));
	TestCharacter_B->GetCapsuleComponent()->SetCapsuleSize(1.f,10.f);
	TestCharacter_B->SetActorLocation(FVector(410.f,210.f,0.0f));
	
	ABattleCharacter* TestCharacter_C = GetWorld()->SpawnActor<ABattleCharacter>();
	TestCharacter_C->GetCapsuleComponent()->SetCollisionProfileName(TEXT("Friend"));
	TestCharacter_C->GetCapsuleComponent()->SetCapsuleSize(1.f,10.f);
	TestCharacter_C->SetActorLocation(FVector(910.f,910.f,0.0f));
	
	ABattleCharacter* TestCharacter_D = GetWorld()->SpawnActor<ABattleCharacter>();
	TestCharacter_D->GetCapsuleComponent()->SetCollisionProfileName(TEXT("Friend"));
	TestCharacter_D->GetCapsuleComponent()->SetCapsuleSize(1.f,10.f);
	TestCharacter_D->SetActorLocation(FVector(1510.f,1510.f,0.0f));
	
	WorldRiskGridMapSubSystem->GetRiskValue(TestCharacter_A->GetActorLocation());
	
	FGridChunk CharacterChunk_A;
	TestTrue(TEXT("GetRiskValue를 진행했기에 해당 위치에 청크가 만들어져야 함"),WorldRiskGridMapSubSystem->GetChunk(TestCharacter_A->GetActorLocation(), CharacterChunk_A));
	
	if (CharacterChunk_A.RiskCellMap.IsEmpty())
	{
		return false;
	}
	
	FIntPoint CharacterCellIdx = CharacterChunk_A.GetCellIdx(TestCharacter_A->GetActorLocation());
	
	TestEqual(TEXT("CharacterA가 위치한 Cell의 위험도는 1이어야 함"), CharacterChunk_A.GetRiskValue(CharacterCellIdx), 1.f);
	TestEqual(TEXT("Character B 또한 A와 같은 Cell이기에 위험도가 업데이트 됨"), CharacterChunk_A.GetRiskValue(TestCharacter_B->GetActorLocation()), 1.0f);
	
	
	CharacterCellIdx = CharacterChunk_A.GetCellIdx(TestCharacter_A->GetActorLocation());
	CharacterCellIdx.X = CharacterCellIdx.X+1;
	TestEqual(TEXT("Character A가 위치한 Cell의 상하좌우 Cell의 위험도는 0.7이어야 함"), CharacterChunk_A.GetRiskValue(CharacterCellIdx), 0.7f);
	
	CharacterCellIdx = CharacterChunk_A.GetCellIdx(TestCharacter_A->GetActorLocation());
	CharacterCellIdx.Y = CharacterCellIdx.Y+1;
	TestEqual(TEXT("Character A가 위치한 Cell의 상하좌우 Cell의 위험도는 0.7이어야 함"), CharacterChunk_A.GetRiskValue(CharacterCellIdx), 0.7f);
	
	CharacterCellIdx = CharacterChunk_A.GetCellIdx(TestCharacter_A->GetActorLocation());
	CharacterCellIdx.X = CharacterCellIdx.X-1;
	TestEqual(TEXT("Character A가 위치한 Cell의 상하좌우 Cell의 위험도는 0.7이어야 함"), CharacterChunk_A.GetRiskValue(CharacterCellIdx), 0.7f);
	
	CharacterCellIdx = CharacterChunk_A.GetCellIdx(TestCharacter_A->GetActorLocation());
	CharacterCellIdx.Y = CharacterCellIdx.Y-1;
	TestEqual(TEXT("Character A가 위치한 Cell의 상하좌우 Cell의 위험도는 0.7이어야 함"), CharacterChunk_A.GetRiskValue(CharacterCellIdx), 0.7f);
	
	CharacterCellIdx = CharacterChunk_A.GetCellIdx(TestCharacter_A->GetActorLocation());
	CharacterCellIdx.X = CharacterCellIdx.X+1;
	CharacterCellIdx.Y = CharacterCellIdx.Y+1;
	TestEqual(TEXT("CharacterB가 위치한 곳이기에 1이어야 함"), CharacterChunk_A.GetRiskValue(CharacterCellIdx), 1.f);
	
	CharacterCellIdx = CharacterChunk_A.GetCellIdx(TestCharacter_A->GetActorLocation());
	CharacterCellIdx.X = CharacterCellIdx.X-1;
	CharacterCellIdx.Y = CharacterCellIdx.Y+1;
	TestEqual(TEXT("Character가 위치한 Cell의 대각 Cell의 위험도는 0.5이어야 함"), CharacterChunk_A.GetRiskValue(CharacterCellIdx), 0.5f);
	
	CharacterCellIdx = CharacterChunk_A.GetCellIdx(TestCharacter_A->GetActorLocation());
	CharacterCellIdx.X = CharacterCellIdx.X+1;
	CharacterCellIdx.Y = CharacterCellIdx.Y-1;
	TestEqual(TEXT("Character가 위치한 Cell의 대각 Cell의 위험도는 0.5이어야 함"), CharacterChunk_A.GetRiskValue(CharacterCellIdx), 0.5f);
	
	CharacterCellIdx = CharacterChunk_A.GetCellIdx(TestCharacter_A->GetActorLocation());
	CharacterCellIdx.X = CharacterCellIdx.X-1;
	CharacterCellIdx.Y = CharacterCellIdx.Y-1;
	TestEqual(TEXT("Character가 위치한 Cell의 대각 Cell의 위험도는 0.5이어야 함"), CharacterChunk_A.GetRiskValue(CharacterCellIdx), 0.5f);
	
	CharacterCellIdx = CharacterChunk_A.GetCellIdx(TestCharacter_A->GetActorLocation());
	CharacterCellIdx.X = CharacterCellIdx.X+2;
	TestEqual(TEXT("CharacterB에 의해 0.3이 아닌 0.5의 위험도를 가짐"), CharacterChunk_A.GetRiskValue(CharacterCellIdx), 0.5f);
	
	CharacterCellIdx = CharacterChunk_A.GetCellIdx(TestCharacter_A->GetActorLocation());
	CharacterCellIdx.X = CharacterCellIdx.X-2;
	TestEqual(TEXT("Character가 위치한 Cell과 2씩 먼 Cell의 위험도는 0.3이어야 함"), CharacterChunk_A.GetRiskValue(CharacterCellIdx), 0.3f);
	
	CharacterCellIdx = CharacterChunk_A.GetCellIdx(TestCharacter_A->GetActorLocation());
	CharacterCellIdx.Y = CharacterCellIdx.Y+2;
	TestEqual(TEXT("CharacterB에 의해 0.3이 아닌 0.5의 위험도를 가짐"), CharacterChunk_A.GetRiskValue(CharacterCellIdx), 0.5f);
	
	CharacterCellIdx = CharacterChunk_A.GetCellIdx(TestCharacter_A->GetActorLocation());
	CharacterCellIdx.Y = CharacterCellIdx.Y-2;
	TestEqual(TEXT("Chunk의 범위 밖이기에 위험도가 0으로 계산됨"), CharacterChunk_A.GetRiskValue(CharacterCellIdx), 0.0f);

	FGridChunk CharacterChunk_C;
	TestTrue(TEXT("C의 경우 A의 청크 내부에 있기에 A의 주변에 없어도 업데이트 됨"), WorldRiskGridMapSubSystem->GetChunk(TestCharacter_C->GetActorLocation(), CharacterChunk_C));
	TestEqual(TEXT("따라서 C 위치의 RiskValue는 1이 나와야 함"), WorldRiskGridMapSubSystem->GetRiskValue(TestCharacter_C->GetActorLocation()),  1.0f);
	
	FGridChunk CharacterChunk_D;
	TestFalse(TEXT("D의 경우 A의 청크 밖에 있기 때문에 같이 업데이트되지 않아야 함."),WorldRiskGridMapSubSystem->GetChunk(TestCharacter_D->GetActorLocation(), CharacterChunk_D));
	
	if (!CharacterChunk_D.RiskCellMap.IsEmpty())
	{
		return false;
	}
	
	WorldRiskGridMapSubSystem->UpdateRisk(TestCharacter_A->GetActorLocation());
	
	TestTrue(TEXT("UpdateRisk의 경우 주변의 청크까지 모두 업데이트를 진행하기에 D도 업데이트 되어야 함"),WorldRiskGridMapSubSystem->GetChunk(TestCharacter_D->GetActorLocation(), CharacterChunk_D));
	TestEqual(TEXT("업데이트 된 D 위치의 RiskValue는 1이 나와야 함"), WorldRiskGridMapSubSystem->GetRiskValue(TestCharacter_D->GetActorLocation()), 1.0f);

	return true;
}


#endif

