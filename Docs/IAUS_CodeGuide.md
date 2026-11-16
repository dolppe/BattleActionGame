# Utility AI 코드 샘플

Utility AI - IAUS 시스템의 파일별 역할과 실행 흐름을 정리했습니다. 각 링크를 통해 프로젝트 내 실제 소스 코드를 확인할 수 있습니다.

## 시스템 개요

몬스터 자신의 상태와 주변 상황을 바탕으로 행동과 대상을 선택하는 Utility AI-IAUS(Infinite Axis Utility System) 시스템입니다.

### 상황 정보와 평가 항목

- 상황 정보: 자신의 체력, 전투 경과 시간 등 현재 상황에 대한 정보.
- 평가 항목(Consideration): 상황 정보를 바탕으로 행동 평가에 사용하기 위해 정의한 정보.  
자신의 체력 비율, 현재 위협 상태, 적에 의한 압박감 등이 있으며, 각 항목의 계산 결과를 평가값이라고 합니다.  
평가값은 0~1 범위로 정규화하여 사용합니다.

### Axis와 행동 점수

- 각 행동에는 여러 Axis가 설정됩니다. Axis에는 평가 항목과 해당 평가값을 점수로 변환하는 함수 설정을 가집니다.  
예를 들어 같은 체력 평가값이라도, 도주 행동에서는 체력이 낮을수록 높은 점수를, 공격 행동에서는 체력이 높을수록 높은 점수를 부여할 수 있습니다.

- Axis는 평가값의 형태에 따라서 다음과 같이 구분됩니다.

| 구분 | 평가 방식 | 예시 |
|---|---|---|
| Single Axis | 하나의 평가값을 점수로 변환 | 자신의 체력, 전투 지속 시간 |
| Multi Axis | 여러 후보의 평가값을 각각 점수로 변환 | 적별 거리·체력. 지역별 유리함 등 |

- Multi Axis는 같은 후보에 대한 점수들을 곱하여 후보별 종합 점수를 계산하고, 대상 유형별 최고 점수 후보를 선택합니다.  

### 행동 선택과 실행

행동 점수는 다음과 같이 계산합니다.

- 행동 점수 = 행동 가중치 x 단일 대상 Axis 점수들의 곱 x 최적 대상 점수

실행 요건을 만족하는 행동 중 최종 점수가 가장 높은 행동을 선택하여 실행합니다. 


## 파일별 역할

| 클래스 | 역할 | 코드 |
|---|---|---|
| `UBattleUtilityAIData` | 사용할 평가 항목, 행동별 Axis 구성과 함수 계수, 실행 조건에 대한 데이터.   함수 프리셋 적용과 에디터 데이터 검증도 수행. | [헤더](https://github.com/dolppe/BattleActionGame/blob/main/Source/BattleActionGame/Enemy/UtilityAI/BattleUtilityAIData.h) · [cpp](https://github.com/dolppe/BattleActionGame/blob/main/Source/BattleActionGame/Enemy/UtilityAI/BattleUtilityAIData.cpp) |
| `UConsiderationFactors` | 상황 정보 수집과 평가 항목별 계산 함수와 평가값 관리를 담당. AI에서 사용할 항목의 계산 함수를 등록하고, 평가값을 계산, 저장하여 Action이 활용할 수 있게함. | [헤더](https://github.com/dolppe/BattleActionGame/blob/main/Source/BattleActionGame/Enemy/UtilityAI/BattleConsiderationFactors.h) · [cpp](https://github.com/dolppe/BattleActionGame/blob/main/Source/BattleActionGame/Enemy/UtilityAI/BattleConsiderationFactors.cpp) |
| `UBattleUtilityAxis` | 평가 항목과 변환 함수 설정을 보관, 전달받은 평가값에 함수를 적용하여 점수 계산. | [헤더](https://github.com/dolppe/BattleActionGame/blob/main/Source/BattleActionGame/Enemy/UtilityAI/BattleUtilityAxis.h) · [cpp](https://github.com/dolppe/BattleActionGame/blob/main/Source/BattleActionGame/Enemy/UtilityAI/BattleUtilityAxis.cpp) |
| `UBattleUtilityAction` | Axis 점수를 결합하여 행동 점수와 유형별 최적 대상을 계산. 파생 클래스에서 구체적인 행동을 구현할 Start, Tick, End 인터페이스 제공. | [헤더](https://github.com/dolppe/BattleActionGame/blob/main/Source/BattleActionGame/Enemy/UtilityAI/Actions/BattleUtilityAction.h) · [cpp](https://github.com/dolppe/BattleActionGame/blob/main/Source/BattleActionGame/Enemy/UtilityAI/Actions/BattleUtilityAction.cpp) |
| `UBattleUtilityAIComponent` | 설정 데이터를 바탕으로 관련 객체를 생성 및 초기화, 상황 갱신과 행동 평가, 행동 선택 등의 실행 흐름을 담당. | [헤더](https://github.com/dolppe/BattleActionGame/blob/main/Source/BattleActionGame/Enemy/UtilityAI/BattleUtilityAIComponent.h) · [cpp](https://github.com/dolppe/BattleActionGame/blob/main/Source/BattleActionGame/Enemy/UtilityAI/BattleUtilityAIComponent.cpp) |

## 실행 흐름

### 1. 설정 데이터와 객체 생성 및 초기화

설정 데이터를 바탕으로 ConsiderationFactors와 Action을 생성 및 초기화합니다.
Factors는 사용할 평가 항목과 계산 함수를 연결하고,
Action은 설정된 Axis를 생성합니다.

- [UBattleUtilityAIData](https://github.com/dolppe/BattleActionGame/blob/2f33fdb141fd181c42a87bb7b59383d523e329ff/Source/BattleActionGame/Enemy/UtilityAI/BattleUtilityAIData.h#L140)
- [UBattleUtilityAIComponent::BeginPlay](https://github.com/dolppe/BattleActionGame/blob/2f33fdb141fd181c42a87bb7b59383d523e329ff/Source/BattleActionGame/Enemy/UtilityAI/BattleUtilityAIComponent.cpp#L15)
- [UConsiderationFactors::InitConsiderFunction](https://github.com/dolppe/BattleActionGame/blob/2f33fdb141fd181c42a87bb7b59383d523e329ff/Source/BattleActionGame/Enemy/UtilityAI/BattleConsiderationFactors.cpp#L776)
- [UBattleUtilityAction::InitAction](https://github.com/dolppe/BattleActionGame/blob/2f33fdb141fd181c42a87bb7b59383d523e329ff/Source/BattleActionGame/Enemy/UtilityAI/Actions/BattleUtilityAction.cpp#L146)

### 2. 실행 상태 확인 및 행동 선택 요청

TickComponent에서 현재 행동을 갱신합니다. 행동이 완료되면 종료하고, 실행 중인 행동이 없으면 SelectBestAction으로 다음 행동을 선택합니다.

- [UBattleUtilityAIComponent::TickComponent](https://github.com/dolppe/BattleActionGame/blob/2f33fdb141fd181c42a87bb7b59383d523e329ff/Source/BattleActionGame/Enemy/UtilityAI/BattleUtilityAIComponent.cpp#L46)
- [UBattleUtilityAIComponent::SelectBestAction](https://github.com/dolppe/BattleActionGame/blob/2f33fdb141fd181c42a87bb7b59383d523e329ff/Source/BattleActionGame/Enemy/UtilityAI/BattleUtilityAIComponent.cpp#L89)

### 3. 상황 정보 수집과 평가값 갱신

SelectBestAction의 도입부에서,
ConsiderationFactors에서 상황 정보를 수집하고, 등록된 계산 함수로 공통 평가값을 갱신합니다.

- [UBattleUtilityAIComponent::CollectConsiderFactors](https://github.com/dolppe/BattleActionGame/blob/2f33fdb141fd181c42a87bb7b59383d523e329ff/Source/BattleActionGame/Enemy/UtilityAI/BattleUtilityAIComponent.cpp#L79)
- [UConsiderationFactors::CollectConsiderationData](https://github.com/dolppe/BattleActionGame/blob/2f33fdb141fd181c42a87bb7b59383d523e329ff/Source/BattleActionGame/Enemy/UtilityAI/BattleConsiderationFactors.cpp#L854)
- [UConsiderationFactors::CalcConsiderationFactors](https://github.com/dolppe/BattleActionGame/blob/2f33fdb141fd181c42a87bb7b59383d523e329ff/Source/BattleActionGame/Enemy/UtilityAI/BattleConsiderationFactors.cpp#L825)

### 4. 행동, 대상 평가 및 실행

FindBestActionIndex에서 실행 조건을 확인하고, 각 Action의 EvaluateScore로 행동 점수와 최적 대상을 계산합니다.  
실행 가능한 행동 중 점수가 가장 높은 행동을 선택하고, SelectBestAction에서 활성화하여 실행합니다.

- [UBattleUtilityAIComponent::FindBestActionIndex](https://github.com/dolppe/BattleActionGame/blob/2f33fdb141fd181c42a87bb7b59383d523e329ff/Source/BattleActionGame/Enemy/UtilityAI/BattleUtilityAIComponent.cpp#L123)
- [UBattleUtilityAction::EvaluateScore](https://github.com/dolppe/BattleActionGame/blob/2f33fdb141fd181c42a87bb7b59383d523e329ff/Source/BattleActionGame/Enemy/UtilityAI/Actions/BattleUtilityAction.cpp#L179)

