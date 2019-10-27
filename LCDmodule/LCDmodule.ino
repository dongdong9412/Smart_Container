/*  Arduino Mega의 경우 LCD의 SDA = 20, SCL = 21에 연결
    encoderInterruptPin과 encoderSwitchPin은 보드에 interrupt핀에 맞게 설정 필요
    DTH 센서에서 받아온 온도와 습도 값은 temperature과 humidity 값에 저장
*/
#include <SPI.h>
#include <MFRC522.h>
#include <Keypad.h>
#include <Servo.h>
#include <SoftwareSerial.h>
#include <MsTimer2.h>
#include "DHT.h"

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define encoderInterruptPin 19
#define encoderDirectionPin 17
#define encoderSwitchPin 18

//SERVO
#define servoPin 6

//펠티어
#define Mosfet 9

//RFID
#define SS_PIN 53
#define RST_PIN 49

//DHT11
#define DHTPIN A8
#define DHTTYPE DHT11

#define DEBUG 0                   // 로터리 엔코더 없어서 DEBUG용 
LiquidCrystal_I2C Monitor(0x27, 16, 2);
MFRC522 rfid(SS_PIN, RST_PIN);
Servo servo;
SoftwareSerial GPS(14, 15);
DHT dht(DHTPIN, DHTTYPE);

/* State Flag */
volatile bool change = false;
volatile bool set = false;
volatile bool correct = false;
volatile bool door_open = false;
volatile bool reset = false;
volatile bool cooling = false;
volatile bool inputPass = false;

byte ondo[] = {
  B11000,
  B11000,
  B00000,
  B00110,
  B01001,
  B01000,
  B01001,
  B00110
};

volatile float temperature;              // DTH11 센서 온도 값
volatile float humidity;                 // DTH11 센서 습도 값
volatile float setTemp = 30.0;            // 로터리 엔코더로 온도 설정할 값

String SetString = "Set Temp:";
String Thermal = "Temp:";
String Humidity = "Humidity:";

volatile int sec = 0;

//LOCK
int pos = 0;
int count = 0;
volatile int wrong = 0;
int index = 0;

String Password = "1234";
const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
byte rowPins[ROWS] = {32, 33, 34, 35}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {36, 37, 38}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

/* Module Initialization */
void Dial_Init();                 // Pin 설정 함수
void Monitor_Init();              // LCD 초기 설정 함수
void DoorLock_Init();             // 잠금 시스템 설정 함수
void Timer_Init();                // 1초마다 실행하게 해줄 인터럽트 타이머

/* Sensing */
void encoderAct();                // 엔코더 돌릴때의 동작
void switchAct();                 // 엔코더 스위치 눌렀을 때 동작
void Sensing();                   // 센서 값 read
void Door_sensing();              // keypad와 RFID읽어오는 함수

/* Actuator */
void UpdateMonitor();             // LCD의 내용이 변경되었을 때 동작
void door_Lock_Unlock();          // 문 잠금
void LED();                       // 실내등
void Cooler();                    // 펠티어 동작
void Actuating();                 // 상황 별 동작 함수



void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  pinMode(Mosfet, OUTPUT);
  GPS.begin(9600);

  Dial_Init();
  Monitor_Init();
  DoorLock_Init();
  Timer_Init();

}

void loop() {
  // put your main code here, to run repeatedly:
#if DEBUG == 1
  String buff = "";
  while (Serial.available()) {
    buff += (char)Serial.read();
    delay(10);
  }
  if (buff != "") {
    change = true;
    Serial.println(buff);
    if (buff == "+")
      setTemp += 0.1;
    if (buff == "-")
      setTemp -= 0.1;
    if (buff == "set")
      set = !set;
    Serial.println(set);
  }
#endif
  Door_sensing();

  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  change = true;
  if (temperature > setTemp)
    cooling = true;
  else
    cooling = false;

  Actuating();
  UpdateMonitor();
  delay(150);
}

/*       Module Initialization      */
void Dial_Init() {
  pinMode(encoderInterruptPin, INPUT_PULLUP);
  pinMode(encoderDirectionPin, INPUT_PULLUP);
  pinMode(encoderSwitchPin, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(encoderInterruptPin), encoderAct, FALLING);
  attachInterrupt(digitalPinToInterrupt(encoderSwitchPin), switchAct, FALLING);
}

void Monitor_Init() {
  Monitor.begin();
  Monitor.backlight();
  Monitor.createChar(0x01, ondo);

  change = true;
}

void DoorLock_Init() {
  servo.attach(servoPin);
  SPI.begin();
  rfid.PCD_Init();
}

void Timer_Init() {
  MsTimer2::set(5000, Sensing);
  MsTimer2::start();
}
/*       Module Initialization      */

/* Sensing Function */
void encoderAct() {
  if (set) {
    if (digitalRead(encoderDirectionPin)) {
      setTemp += 0.1;
    }
    else {
      setTemp -= 0.1;
    }
  }
  else
    ;
  delay(100);             // debouncing 제거 용
  change = true;
}

void switchAct() {
  set = !set;
  delay(100);             // debouncing 제거 용
  change = true;
}

void Sensing() {
<<<<<<< HEAD
  change = true;

  if (door_open) {
    sec++;
    if (sec >= 2) {
      door_open = false;
      sec = 0;
    }
=======
  if (door_open) {
    sec++;
>>>>>>> 1bbce2ef7c6bcc40b7b76b9f616f9d801145de65
  }

  if (sec > 4) {
    sec = 0;
    door_open = false;
  }
}

void Door_sensing() {
  char key = keypad.getKey();

  if (key) {
    inputPass = true;
    if (key == Password[index]) {
      index++;
      count++;
    }

    else if (key != Password[index]) {
      index++;
      correct = false;
    }

    if (key == '#') {
      correct = false;
      inputPass = false;
      door_open = false;
      reset = true;
      sec = 0;
      index = 0;
      count = 0;
      reset = true;
      inputPass = false;
    }

    if (key == '*') {
      if (!rfid.PICC_IsNewCardPresent()) {
        return;
      }
      if (! rfid.PICC_ReadCardSerial()) {
        return;
      }

      String content = "";

      for (byte i = 0; i < rfid.uid.size; i++) {
        content.concat(String(rfid.uid.uidByte[i] < 0x10 ? "0" : " "));
        content.concat(String(rfid.uid.uidByte[i], HEX));
      }
      content.toUpperCase();

      if (content.substring(1) == "76 F5 3B F8" || content.substring(1) == "F902 B9 55") {
        Monitor.clear();
        Monitor.setCursor(0, 0);
        Monitor.print("Authoried access");
        door_open = true;
<<<<<<< HEAD
        delay(1000);
=======
        delay(1500);
>>>>>>> 1bbce2ef7c6bcc40b7b76b9f616f9d801145de65
      }
      else {
        Monitor.clear();
        Monitor.setCursor(0, 0);
        Monitor.print("Access denied");
<<<<<<< HEAD
        delay(1000);
=======
        delay(1500);
>>>>>>> 1bbce2ef7c6bcc40b7b76b9f616f9d801145de65
      }
      inputPass = false;
    }

    if (index == 4) {
      if (count == 4) {
        correct = true;
        Monitor.clear();
        Monitor.setCursor(0, 0);
        Monitor.print("Authoried access");
        door_open = true;
        delay(1000);
      }
      else {
        correct = false;
        Monitor.clear();
        Monitor.setCursor(0, 0);
        Monitor.print("Access denied");
        delay(1000);
        door_open = false;
      }
      inputPass = false;
      count = 0;
      index = 0;
      inputPass = false;
    }
  }


}
/* Sensing Function */

/* Actuator Function */
void UpdateMonitor() {
  if (reset) {
    Monitor.clear();
    Monitor.setCursor(0, 0);
    Monitor.print("Password Reset");
    delay(3000);
    reset = false;
<<<<<<< HEAD
  }
  if (inputPass) {
    Monitor.clear();
    Monitor.setCursor(0, 0);
    String temp_str = "Password: ";
    for (int i = 0; i < index; i++) {
      temp_str += "*";
    }
    Monitor.print(temp_str);
    change = false;
=======
>>>>>>> 1bbce2ef7c6bcc40b7b76b9f616f9d801145de65
  }
  if (change) {
    if (set) {
      Monitor.clear();
      Monitor.setCursor(0, 0);
<<<<<<< HEAD
      Monitor.print(Thermal + String(temperature));
      Monitor.write(0x01);
      Monitor.setCursor(0, 1);
      Monitor.print(SetString + String(setTemp));
      Monitor.write(0x01);
=======
      Monitor.print("Password: ");
      for (int i = 0; i < index; i++)
        Monitor.print("*");
>>>>>>> 1bbce2ef7c6bcc40b7b76b9f616f9d801145de65
    }
    else {
      Monitor.clear();
      Monitor.setCursor(0, 0);
      Monitor.print(Thermal + String(temperature));
      Monitor.write(0x01);
      Monitor.setCursor(0, 1);
      Monitor.print(Humidity + String(humidity));
      Monitor.print("%");
    }

  }
  change = false;
}

void door_Lock_Unlock() {
  if (door_open)
    servo.write(90);
  else
    servo.write(0);
}

void LED() {
  if (door_open)
    ;
  /* LED On code */
  else
    ;
  /* LED Off code */
}

void Cooler() {
  if (cooling)
    digitalWrite(Mosfet, HIGH);
  else
    digitalWrite(Mosfet, LOW);
}

void Actuating() {
  door_Lock_Unlock();
  LED();
  Cooler();
}
/* Actuator Function */
