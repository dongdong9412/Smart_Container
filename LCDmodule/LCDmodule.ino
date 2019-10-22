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

#define encoderInterruptPin 2
#define encoderDirectionPin 4
#define encoderSwitchPin 3

//SERVO
#define servoPin 6

//펠티어
#define Mosfet = 9

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

volatile bool change = false;
volatile bool set = false;

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

float temperature;              // DTH11 센서 온도 값
float humidity;                 // DTH11 센서 습도 값
volatile float setTemp = 0.0;            // 로터리 엔코더로 온도 설정할 값

String SetString = "Set Temp:";
String Thermal = "Temp:";
String Humidity = "Humidity:";

//LOCK
int pos = 0;
int tru = 0;
int count = 0;
bool door = false;
char Password[4] = {'1', '2', '3', '4'};
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


void Dial_Init();                // Pin 설정 함수
void Monitor_Init();             // LCD 초기 설정 함수
void DoorLock_Init();            // 잠금 시스템 설정 함수
void UpdateMonitor();            // LCD의 내용이 변경되었을 때 동작
void encoderAct();               // 엔코더 돌릴때의 동작
void switchAct();                // 엔코더 스위치 눌렀을 때 동작

void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(9600);
  pinMode(Mosfet, OUTPUT);
  GPS.begin(9600);
  
  Dial_Init();
  Monitor_Init();
  DoorLock_Init();

  temperature = 21.0;
  humidity = 76;

}

void loop() {
  // put your main code here, to run repeatedly:
  String buff = "";
  while (Serial.available()) {
    buff += (char)Serial.read();
    delay(10);
  }

#if DEBUG == 1
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

  UpdateMonitor();
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
void DoorLock_Init(){
  servo.attach(servoPin);
  SPI.begin();
  rfid.PCD_Init();
}
/*       Module Initialization      */

void UpdateMonitor() {
  if (change) {
    if (set) {
      Monitor.clear();
      Monitor.setCursor(0, 0);
      Monitor.print(Thermal + String(temperature));
      Monitor.write(0x01);
      Monitor.setCursor(0, 1);
      Monitor.print(SetString + String(setTemp));
      Monitor.write(0x01);
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
    change = false;
  }
}

void encoderAct() {
  if (digitalRead(encoderDirectionPin)) {
    setTemp += 0.1;
  }
  else {
    setTemp -= 0.1;
  }
  delay(100);             // debouncing 제거 용
  change = true;
}

void switchAct() {
  set = !set;
  delay(100);             // debouncing 제거 용
  change = true;
}
