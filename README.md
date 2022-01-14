# MicroProcessor_NumBaseBallGame
 정규학기 3-1에 진행된 마이크로 프로세서 수업 결과물

<img width="400" src="https://user-images.githubusercontent.com/86091469/149492974-a26a70b2-8700-49c1-af01-99ec2fc95c99.jpg"/>


# 사용 환경
KT-M128, ATMega128 트레이닝 킷을 사용함.

AtmelStudio, Visual Studio 2015를 기반으로 만듬.

ComportMaster를 통해 Usart 통신을 함. (http://withrobot.com/data/?mod=document&uid=12)


# Flow Chart
<img width="500" src="https://user-images.githubusercontent.com/86091469/149503891-e58f71c4-81e0-4a2b-936a-1f75802ebdd4.png"/>

# 동작 과정
보드를 실행하면 숫자 야구 게임이 시작 된다.

총 9번의 기회가 주어지고 남은 기회는 DotMatrix위에 표시된다.

정답 입력은 Usart 통신을 통하여 한다.

승리 시 맞추는데 걸린 횟수로 스코어를 책정한다.

패배시 정답을 알려준다.

승패 판별 후 PinB를 통하여 재시작 할 것인지 판단한다.

승리/패배/입력/시작 사운드가 나뉘어져 있다.

# 유의 사항
최종본 파일이 분실 되어, 파일 분할이 되어 있지 않은 original.c 파일을 기반으로 코드를 재작성 하였으나 보드가 없어 컴파일 및 검사를 하지 못하였음.

만약 문제가 발생 할 시, original.c 파일을 이용하길 바람.