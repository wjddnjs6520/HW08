## HW08
- BP에 사용된 메쉬, 사운드 등의 리소스 중 직접 만든 것이 아니라면 올리지 않았습니다.
  
#### 웨이브 구성
- 레벨 3번 이동하는게 아닌 한 레벨에서 웨이브로 돌아가도록 설정.

- 레벨 대신 웨이브를 위한 WaveGameState의 새로운 클래스 생성 

#### HUD & 메뉴 UI 리뉴얼
- 게임 시작 화면(메인 메뉴)
  - Get Coin!(버튼): 게임 시작
    - 가운데 부분 Text 및 색 변경
  - Exit(버튼): 게임 종료
    - 오른쪽 위 추가
  - 배경: 코인과 지뢰 아이템 배치. 맵의 분위기에 맞춰 경찰이 잡으러 온 느낌으로 화면 구성.
  <img width="1920" height="1040" alt="image" src="https://github.com/user-attachments/assets/717fef79-cae7-46ec-86ed-570273e21f92" />
  - Retry Get Coin!!(버튼): 다시 시작(게임 오버 또는 Tap 키)
    - 기존에는 Tap 키를 눌렀을 때도 게임 오버 화면이 나오게 설정된 것을 Tap키와 게임 오버 시 화면을 분리
    - Tap: 총 점수와 다시시작 버튼

     ![](https://velog.velcdn.com/images/wjddnjs6520/post/36c78cc9-fb53-451a-82e2-71ba49dc2cb0/image.PNG)

    - 게임 오버: 게임 오버 텍스트 애니매이션 + 총 점수와 다시시작 버튼
    ![](https://velog.velcdn.com/images/wjddnjs6520/post/50580b3d-004f-4156-a9c7-cd31f5c7d934/image.PNG)

- 게임 플레이 화면
  - 위쪽 중앙1(타이머+웨이브): 시간이 지날수록 왼쪽으로 이동하며 웨이브 시작 시간을 시작적으로 주게 함.
  - 위쪽 중앙2(카운트다운): 현재 웨이브가 10초 이하로 남았을 경우 카운트 다운 시작
  - 왼쪽 중앙(점수 및 코인): 현재 점수와 현재 모은 코인의 개수 표시
  - 캐릭터 오른쪽(특정 아이템 사용): 특정 아이템과 오버랩 시 추가되는 위젯. 사용하거나 오버랩이 종료되면 위젯 제거
  - 아래쪽 중앙(HP): 현재 캐릭터의 HP를 보여주는 위젯. 바 형태로 제작
    - 해당 UI는 나중에 캐릭터 머리 위에 3D로 뜨도록 변경할 예정
<img width="1920" height="1040" alt="3  HUD 웨이브 남은 시간과 다음 웨이브 현황 애니매이션" src="https://github.com/user-attachments/assets/441724cc-dc3c-4ce9-9117-afe612a76b73" />


#### 아이템 상호작용 로직 고도화
- 기존 아이템 변경
  - 지뢰 아이템: 기존 터지는 시간이 5초였는데 2초로 터지는 시간 감소
- 새로운 부정적 아이템 효과를 최소 두 가지 추가
  - 얼음주스: 몸을 움직이지 못하는 아이템
    - 태그 사용
    - State.Frozen 이 아닐 시에만 입력 키 적용되도록 설정
    - 해당 상태 시 살짝 파란 화면을 위젯에 추가. 시간 지나면 삭제
  <img width="522" height="370" alt="10  아이스 주스" src="https://github.com/user-attachments/assets/08ed4f51-dd28-4547-a1a1-62ed4cc15232" />
<img width="1920" height="1040" alt="15  아이스 주스 UI" src="https://github.com/user-attachments/assets/6ba5cd35-9212-4170-bf23-0b3fac145d36" />

  - 실명주스: 시야가 어두워지는 아이템
    - 오버랩 시 점점 어두워졌다가 밝아지는 위젯 추가하고 일정시간후 삭제 설정
  <img width="450" height="393" alt="13  블랙 주스" src="https://github.com/user-attachments/assets/2ef57381-7e18-4ec2-8297-b77f4878dace" />
  <img width="1920" height="1040" alt="14  블랙 주스UI" src="https://github.com/user-attachments/assets/c5a94af4-738b-4a68-8762-9653769ae803" />

  - 자석: 일정 범위 내의 코인이 플레이어 방향 끌어당겨짐.
    - 사용 시 태그가 코인인 액터의 위치를 플레이어 위치와 액터의 위치를 빼 방향을 구하고 해당 방향으로 일정 시간 옮겨짐.
  <img width="856" height="603" alt="12  마그네틱" src="https://github.com/user-attachments/assets/687c2229-0a3d-4b88-a74c-fa34bd700691" />

  - 슬로우 마인
    - 데미지 대신 캐릭터의 이동 속도를 느려지게하는 지뢰.
    <img width="433" height="400" alt="11  슬로우 마인" src="https://github.com/user-attachments/assets/34a4a6ed-49e2-4be7-a54c-5efd7a9cdc9c" />

      
#### 웨이브별 환경의 변화
- 웨이브 별 스폰 아이템 확률 및 개수 조정
<img width="706" height="171" alt="7  wave1 데이터" src="https://github.com/user-attachments/assets/16488c90-877e-42f8-9ad1-544cb02961dc" />
<img width="702" height="175" alt="8  wave2 데잍" src="https://github.com/user-attachments/assets/c7c2bee2-01e6-4151-9ed4-32b5f90fd3ab" />
<img width="701" height="167" alt="9  wave3 데이터" src="https://github.com/user-attachments/assets/e09464fe-81ef-47b1-97c9-fd9e1f4f7c7b" />

- wave 2
  - 일정 시간마다 캐릭터 앞에 5초간 장애물 생성(돌아가야 되므로 시간 소모)
- wave 3
  - 장애물 옆에 슬로우 마인 생성

#### 플레이 영상
[HW08 플레이 영상](https://drive.google.com/file/d/15hvn4S4HHwLQ71lkm11QfuvbkDLNvd_0/view?usp=sharing)

#### 참고자료
- 출처: 팀스파르타 내일배움캠프 
