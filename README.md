# BattleActionGame

Unreal Engine 5.1과 C++로 개발한 전용 서버(Dedicated Server) 기반 멀티플레이 액션 RPG입니다.


<img width="2560" height="1440" alt="JustGuard순간" src="https://github.com/user-attachments/assets/a04043c1-538c-4527-99fd-2e98fbf921de" />




## 프로젝트 자료

- [플레이 영상](https://youtu.be/lwwzWyw08Vk)
- [Notion - 전체 기능 및 상세 설명](https://app.notion.com/p/133ed0548bcd8320833e8175eab82baf)
- [PDF 포트폴리오](https://github.com/user-attachments/files/34193687/_.pdf)


## 개발 정보

- 개발 환경 : Unreal Engine 5.1, C++
- 개발 기간 : 1년
- 개발 인원 : 1명

## 주요 구현

### 1. 전투 시스템

- 플레이어와 적의 공격, 방어, 피격 반응 등을 처리하는 액션 전투 구조입니다.
### 2. 부위 시스템

- 피격 부위의 체력과 표면 상태를 관리하고, 손상 정도에 따라 상태 변경과 부위파괴를 처리하는 시스템입니다.

### 3. Utility AI - IAUS 시스템

- AI의 각 행동이 현재 상황에 얼마나 적합한지를 점수로 계산하여 행동을 선택하는 UtilityAI 방식을 사용했습니다.
- UtiityAI 방식 중 여러 평가 요소를 조합하여 행동 점수를 계산하는 IAUS(Infinite Axis Utility System) 구조를 적용했습니다.

### 4. 맵 정보를 활용한 AI 시스템

- 주변 적의 위험도를 고려하여 이동 경로를 선택하고, 지형의 유불리에 따라 위치를 재조정하는 AI를 구현했습니다.

### 5. 스탯 데이터 관리 자동화

- Python으로 언리얼의 AttributeSet 정보를 수집하여 엑셀에 반영하고, 엑셀에서 편집한 스탯을 다시 DataTable에 적용하는 과정을 자동화했습니다.

### 6. 자동화 테스트 및 CI 구축

- C++ 자동화 테스트, 맵 기반 통합 테스트, 에셋 테스트를 구성하고, CI를 통해 빌드와 테스트를 자동으로 실행하도록 했습니다.
- 단, CI의 경우 GitHub Actions의 Self-hosted Runner를 사용했기에 보안을 위해 비공개 저장소에서만 진행됩니다.

## 에셋 및 실행 안내

재배포가 제한된 에셋은 포함하지 않아, 저장소만으로는 전체 프로젝트를 실행할 수 없습니다.

플레이 및 구현 결과는 플레이 영상과 Notion에서 확인할 수 있습니다.

