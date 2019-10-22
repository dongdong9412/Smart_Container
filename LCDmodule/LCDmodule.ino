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

void MonitorInit();             // LCD 초기 설정 함수
void UpdateMonitor();           // LCD의 내용이 변경되었을 때 동작
void encoderAct();              // 엔코더 돌릴때의 동작
void switchAct();               // 엔코더 스위치 눌렀을 때 동작

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  MonitorInit();

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
    if(buff == "+")
      setTemp += 0.1;
    if(buff == "-")
      setTemp -= 0.1;
    if(buff == "set")
      set = !set;
    Serial.println(set);
  }
  #endif

  UpdateMonitor();
}
void MonitorInit() {
  pinMode(encoderInterruptPin, INPUT_PULLUP);
  pinMode(encoderDirectionPin, INPUT_PULLUP);
  pinMode(encoderSwitchPin, INPUT_PULLUP);
  
  attachInterrupt(digitalPinToInterrupt(encoderInterruptPin), encoderAct, FALLING);
  attachInterrupt(digitalPinToInterrupt(encoderSwitchPin), switchAct, FALLING);

  Monitor.begin();
  Monitor.backlight();
  Monitor.createChar(0x01, ondo);
  
  change = true;
}


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
