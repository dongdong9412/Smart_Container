# Readme

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

   

   

   