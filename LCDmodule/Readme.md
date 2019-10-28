# Readme

2019/10/22

1. 상단의 헤더파일 모두 복사

2. define 복사 후 알맞은 핀으로 할당

3. DEBUG 0으로 수정

4. temperature변수와 humidity 변수는 따로 선언 할 필요 없이 DTH11 온도센서 값을 받아오는 변수랑 맞추면 됨

5. 아래와 같은 구조로 집어넣으면 됨

   ```c
   void setup(){
       MonitorInit();
   }
   void loop(){
       UpdateMonitor();
   }
   
   void encoderAct(){
       복사
   }
   void switchAct(){
       복사
   }
   ```

   ***volatile bool 변수는 이름이 겹칠 경우 이름 조정이 필요(change, set)***

   ***만약 로터리 엔코더 돌아가는 방향이 반대로 동작할 경우에는  아래와 같이encoderAct() 함수 안에서 if 조건문 앞에 !를 붙여주면 됨***

   ```c
   if (!digitalRead(encoderDirectionPin)) {
       setTemp += 0.1;
     }
     else {
       setTemp -= 0.1;
     }
   ```

   

   

   ---

   2019-10-23

   **대략적인 알고리즘 작성 완료. ** 
   ***검증 필요. flag 변수 별 state가 잘 돌아가는지 확인 필요.***

   ***암호 입력 시 LCD interface 미구현***

   ---
   
   2019-10-24
   
   ***펠티어 전력 문제로 교체 필요***
   
   ***LCD state 흐름에 따라 출력 오류 존재***
   
   ***Keypad 및 RFID 하드웨어 추가***
   
   ---
   
   2019-10-27 	[1차 배포]  
   
   **전반적인 설계 완료** 
   **안정적으로 동작.**
   **계속 테스트하면서 버그 찾아내는 작업 필요**
   
   ---
   
   2019-10-27 [지수 작업]
   
   **16x4 스펙으로 변경**
   
   **DHT11 Not a Number 오류 처리**
   
   
   
   
   
   

---

2019-10-25

***LCD module 수정***

***flag 변수 state별로 clear 조건 수정***

