# BattleActionGame

데디케이티드 서버를 활용한 **네트워크 액션 RPG 장르**의 프로젝트.

**전투** 시스템, **AI** 시스템에 집중하여 구현.

- UtilityAI - IAUS(Infinite Axis Utility System) 시스템
- 공격 시스템
- 그 외의 시스템  
    - 부위파괴 시스템
    - 회피 시스템

- 프로젝트에 대한 자세한 설명은  
  https://harmonious-soarer-fb1.notion.site/RPG-126ed0548bcd80229a31ee111f0a5fc9 에 있습니다.

## Requirements

- Unreal Engine 5 (v5.1)
- Data Version Control (https://dvc.org/)

## install

-  Git Clone
    ```sh
    git clone https://github.com/dolppe/LyraBattleRoyalGame.git
    ```

- DVC 적용 (Content 폴더 가져오는 방법)
    - DVC Install  
    https://dvc.org/doc/install

    - DVC 적용 (git이 적용된 상태에서 가능.)
    ```sh
    dvc pull
    ```

- Unreal Engine Version 세팅
    - LyraBattleRoyalGame.uproject 파일에 우클릭 하여 Swith Unreal Engine Version 선택.
    - 5.1 버전 선택 => Generate Project File 진행.
    - 이후 LyraBattleRoyalGame.uproject을 더블 클릭하고, rebuild 진행. (기다리면 자동 실행됨.)  
    => 혹은 직접 빌드하여 실행하는 것도 가능.

## Feature

- 각 기능별 특징과 구현, 구조에 대한 설명.

### UtilityAI -IAUS(Infinite Axis Utility System) 시스템

- IAUS 시스템은 주변 환경 정보를 하나의 축(Axis)으로 변환하여, 행동(Action)에 대한 점수를 매기고, 행동들 중 가장 평가치가 높은 행동을 선택하는 시스템입니다.
- 점수 계산 방식
    - UtilityAIComponent에서 거리, HP 등 주변 데이터를 수집합니다.
    - 각 Action에 대해서 EvaluateValue를 호출되며, 저장된 Factor에 따라서 Action 내부의 Axis들이 값을 가져와서 계산합니다.
    - Action의 값에 설정된 가중치를 곱하게 됩니다. 이후 모든 Action의 Score를 비교하고, 가장 높은 Score의 Action을 선택하여 행동합니다.
- UtilityAIData (도주 Action에 대한 정보)
![IAUS_Data](https://github.com/user-attachments/assets/c3ed5358-3304-474d-a936-1ea3a1d9d5bc)  

- IAUS의 클래스 다이어그램
![IAUS_ClassDiagram](https://github.com/user-attachments/assets/46f802f1-9912-48cd-8447-5236d8f827b9)

- IAUS 설계의 특징
    - Strategy 패턴  
        - IAUS에 적용해서, 런타임에 Action을 선택하고, 선택된 Action에 따라서 실시간으로 동작이 바뀌도록 하여 Strategy 패턴을 적용했습니다.
    - Command 패턴
        - Action마다 다양한 Axis가 존재하며, AIComponent에서 관련 값을 가지고 있다가 함수를 통해 가공하는데, 이를 객체화하여 추가적은 Axis가 생겨도 모듈성을 유지하도록 Command 패턴을 적용했습니다.
    - Composite 패턴
        - Axis는 단일 축을 가진 객체와 Targets이나 Spots에 대한 복수의 축을 가진 복합 객체로 나뉩니다. 그렇기에 단일 객체와 복합 객체를 한번에 포함하고, 이를 동일한 방식으로 처리할 수 있는 Composite 패턴을 사용했습니다.
    - 데이터 기반 설계
        - IAUS는 지속적으로 Action과 Axis를 추가해야 하고, 이에 대해 가중치나 평가함수를 부여해야 하기 때문에 수정과 테스트가 편하도록 데이터 기반으로 설계했습니다.

### 공격 시스템

- 중요한 점
    - 다양한 공격 모드, Hitcheck 방식 등에 확장성 있게 대응할 수 있는 구조가 필요합니다.
    - 공격 자체에도 여러 종류가 있으니 이에 대해 모아서 데이터로 처리할 수 있는 구조가 필요합니다.
    - 사용자의 경험이 가장 먼저 느껴질 수 있는 부분이라서, 최적화, 응답속도 등에 잘 대응할 수 있어야 합니다.
    - 네트워크 환경에서 이뤄지는 공격의 흐름이 늦거나 끊어지는 것을 최대한 방지해야 합니다.

- CombatData
![Comba_tData](https://github.com/user-attachments/assets/53dd4791-54b4-4d6d-9f5a-0e410dc923a5)

- 공격 시스템의 클래스 다이어그램
![Combat_ClassDiagram](https://github.com/user-attachments/assets/40042660-fcc5-4baf-aec8-3b67148b7361)  

- 공격 시스템 설계의 특징
    - Template Method 패턴
        - 다양한 공격 시스템에 같은 구조로 대응하기 위해 전체적인 흐름은 상위 클래스에서 구현하고, 상세한 구현은 하위 클래스에서 재정의하도록 Template Method 패턴을 사용했습니다.
    - GAS (GameplayAbilitySystem) 기반 설계
        - GameplayAbilitySystem을 기반으로 하여 공격 시스템 간의 연관을 줄이고, GA (GameplayAbility)와 AT (AbilityTask) 등의 시스템을 활용하여 AnimationPlay, HitCheck, Hit에 대한 결과 처리가 최대한 독립적으로 동작하게 했습니다.
    - 데이터 기반 설계
        - 다양한 공격들을 효율적으로 개발하기 위하여 데이터 기반으로 공격 시스템을 설계했습니다.
        - 각 공격 타입을 하나의 데이터로 관리할 수 있어서, 해당 캐릭터가 가지고 있는 공격에 대한 정보를 한 눈에 보고, 관리하기 편합니다.
    - 콤보 시스템
        - 콤보 공격은 CombatComponent를 통해 동작하도록 하여 독립적으로 동작할 수 있게 설계했습니다.
    - 공격의 흐름
        - 네트워크는 갑자기 느려지거나 할 수 있기 때문에, 클라이언트에서 서버의 판정과 명령을 그저 기다리는 것이 아니라 능동적으로 행동할 수 있게 설계했습니다.

- HitCheck 시스템
    - HitCheck 방식은 여러가지가 있고, 각 장단점이 존재합니다.
        - 무기에 Collision을 부착해서 이것으로 HitCheck하는 방식.
            - 공격 타이밍을 어느정도 정밀하게 측정 가능합니다.
            - 무기가 아닌 발차기와 같은 공격에 대해서 대응하기 힘듭니다.
        - 애니메이션에 맞는 타이밍에 Collision을 만들어서 순간 HitCheck하는 방식.
            - 성능적으로 매우 좋은 대신에 정밀한 HitCheck가 불가능합니다.
        - 공격이 시작되면 매 Tick마다 Hit를 체크하는 방식
    - 여러 Hitcheck 시스템을 공격마다 다르게 활용할 수 있게 확장성 있는 구조를 설계했습니다.
    - 다양한 공격에 대한 처리를 위해 해당 프로젝트에서 주로 사용하는 것은 다음과 같은 방식입니다.
        - CombatData를 통해 공격에 사용될 Socket의 Start와 EndName을 저장합니다.
        - 공격이 시작되면 AbilityTask를 활용하여 TickTask를 돌면서 매 틱마다 HitCheck를 합니다.
        - 장점
            - 사용자가 애니메이션에 맞는 공격 타이밍을 받을 수 있습니다.
            - 무기가 아닌 주먹이나 발차기 같은 경우에도 CombatData를 조절해서 HitCheck할 수 있습니다.
        - 단점
            - TickTask는 사용자의 컴퓨터 성능에 따라서 초당 호출되는 횟수가 다릅니다.
            - 성능적인 문제가 있습니다.
        - 단점에 대한 해결
            - TickTask에서 DeltaTime이 많이 걸린 시간에 이전 무기의 위치에 대해서 선형 보간을 하는 것입니다.
                - 기존의 HitCheck와 수직으로 선형보간을 진행합니다.
                


