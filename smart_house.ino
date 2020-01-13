#include "Keypad.h"
#include <Wire.h>
#include "RTClib.h"
#include <string.h>
#include <stdio.h>
#include "notes.h"
#include <Servo.h>
#include <LiquidCrystal.h>

const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] ={
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'#','0','*'}
};
// ---------------------- PINS DECLARATION ---------------------- //
const int setAlarmButtonPin = 10;
const int resetAlarmButtonPin = 13;

RTC_DS3231 rtc;
 
const int buz = 11;

const int resistorPin = A0;

const int servoPin = 12;

const int ledHousePin = 9;

byte rowPins[ROWS] = {22, 23, 24, 25};
byte colPins[COLS] = {26, 27, 28};

LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

// ---------------------- VARIABLES DECLARATION ---------------------- //
int melody[] = {
  NOTE_FS5, NOTE_FS5, NOTE_D5, NOTE_B4, NOTE_B4, NOTE_E5, 
  NOTE_E5, NOTE_E5, NOTE_GS5, NOTE_GS5, NOTE_A5, NOTE_B5, 
  NOTE_A5, NOTE_A5, NOTE_A5, NOTE_E5, NOTE_D5, NOTE_FS5, 
  NOTE_FS5, NOTE_FS5, NOTE_E5, NOTE_E5, NOTE_FS5, NOTE_E5
};

int noteDuration[] = {
  8, 8, 8, 4, 4, 4, 
  4, 5, 8, 8, 8, 8, 
  8, 8, 8, 4, 4, 4, 
  4, 5, 8, 8, 8, 8
};
int songLength = sizeof(melody)/sizeof(melody[0]);

char alarmArray[5] = {};
char hourArray[2] = {};
char minuteArray[2] = {};

int actualHour;
int actualMin;
int alarmHour;
int alarmMinute;

int alarmTriggered = 0;

uint32_t period = 0.5 * 60000L;

DateTime now;

int setAlarmButtonState = 0;
int resetAlarmButtonState = 0;
int flag = 0;

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

int resistorValue;

Servo servoShutter;

void setup()
{
  Serial.begin(9600);
  pinMode(setAlarmButtonPin, INPUT_PULLUP);
  pinMode(resetAlarmButtonPin, INPUT_PULLUP);
  pinMode(buz, OUTPUT);
  pinMode(ledHousePin, OUTPUT);
  pinMode(resistorPin, INPUT);
  servoShutter.attach(servoPin);
  delay(3000);

  if(!rtc.begin()){
    Serial.println("Couldn't find RTC");
    while(1);
  }

  lcd.begin(16,2);
  
  Serial.println("Welcome to the smart house");
  lcd.print("Welcome to the");
  lcd.setCursor(2,1);
  lcd.print("smart house");
  delay(2000);
}
 
void loop()
{
  now = rtc.now();
  actualHour = now.hour();
  actualMin = now.minute();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("The time is :");
  lcd.setCursor(5,1);
  lcd.print(actualHour);
  lcd.setCursor(7,1);
  lcd.print(":");
  lcd.setCursor(8,1);
  if(actualMin < 10){
    lcd.print(0);
  }
  lcd.print(actualMin);
  
  setAlarmButtonState = digitalRead(setAlarmButtonPin);
  resetAlarmButtonState = digitalRead(resetAlarmButtonPin);
  resistorValue = analogRead(resistorPin);

  
  if(setAlarmButtonState == 0){
    if(alarmHour == 0 && alarmMinute == 0){
      if(flag == 0){
        lcd.clear();
        lcd.print("Get in alarm mode");
        Serial.println("Get in alarm mode");
        setAlarmTime();
      }
      else if(flag == 1){
        lcd.clear();
        lcd.print("Alarm set :");
        lcd.setCursor(5,1);
        Serial.println("Get and display time set :");
        flag = 0;
        getHour();
        getMinute();
        delay(2000);
      }
    } else{
      lcd.clear();
      lcd.print("Alarm already set !");
      Serial.println("Alarm alredy set");
    }
    
  }
  delay(200);

  if(actualHour == alarmHour && actualMin == alarmMinute && alarmTriggered == 0){
    Serial.println("Alarm ON");
    lcd.clear();
    lcd.setCursor(3,0);
    lcd.print("ALARM ON !!");
    alarmTriggered = 1;
    buzzer();
  }

  if(resetAlarmButtonState == 0){
    lcd.clear();
    lcd.print("Reset alarm");
    delay(1000);
    Serial.println("Reset Alarm");
    resetVariables();
  }

  Serial.println("RESISTOR VALUE");
  Serial.println(resistorValue);

  if(resistorValue <= 144){
    digitalWrite(ledHousePin, HIGH);
    servoShutter.write(0);
    delay(1000);
  } else if (resistorValue >= 144)
  {
    digitalWrite(ledHousePin, LOW);
    servoShutter.write(90);
    delay(1000);
  }
  delay(500);
  
}

void setAlarmTime(){
  int numberKeyPressed = 0;
  lcd.clear();
  lcd.print("Alarm setup : ");
  lcd.setCursor(5,1);
  while(numberKeyPressed < 4){
    char key = keypad.getKey();
    if(key != NO_KEY){
      Serial.println(key);
      lcd.print(key);
      numberKeyPressed++;
      alarmArray[numberKeyPressed] = key;
    }
  }
  flag = 1;
}

void getHour(){
  byte y=0;
  for(byte i = 1; i < 3; i++){
    Serial.println(alarmArray[i]);
    hourArray[y] = alarmArray[i];
    y++;
  }

  alarmHour = atoi(hourArray);
  Serial.println("Hour set :");
  lcd.print(alarmHour);
  lcd.print(":");
  Serial.println(alarmHour);
}

void getMinute(){
  byte y=0;
  for(byte i = 3; i < 5; i++){
    minuteArray[y] = alarmArray[i];
    y++;
  }

  alarmMinute = atoi(minuteArray);
  Serial.println("Minute set :");
  Serial.println(alarmMinute);
  if(alarmMinute < 10){
    lcd.print(0);
  }
  lcd.print(alarmMinute);
}

void buzzer(){
  for(uint32_t tStart = millis(); (millis()-tStart) < period;){
    for (int thisNote = 0; thisNote < songLength; thisNote++){
      int duration = 1000/ noteDuration[thisNote];
      tone(buz, melody[thisNote], duration);
      int pause = duration * 1.3;
      delay(pause);
      noTone(buz);
    }
  }
}

void resetVariables(){
  alarmHour = 0;
  alarmMinute = 0;
  alarmArray[0] = '\0';
  minuteArray[0] = '\0';
  hourArray[0] = '\0';
  alarmTriggered = 0;
}
