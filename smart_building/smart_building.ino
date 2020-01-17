#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include "RTClib.h"
 
#define SS_PIN 10
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN);
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
SoftwareSerial BTserial(0,1);
RTC_DS3231 rtc;

int led = A0;
int ledHeaterPin = A1;
int ledComputerPin = A2;

int servoPin = 8;
int numberOfScan = 0;

String dataHouse;
String data = "";

int actualHour;
int actualMinute;

Servo servo;
DateTime now;
DateTime arrivalTime; 

int brightness = 0;
int fadeAmount = 5;
int x =0;
int heaterTriggered = 0;
void setup() 
{
  Serial.begin(9600);
  BTserial.begin(38400);
  SPI.begin();
  lcd.begin(16,2);
  Wire.begin();
  
  mfrc522.PCD_Init();
  servo.attach(servoPin);

  pinMode(led, OUTPUT);
  pinMode(ledHeaterPin, OUTPUT);
  pinMode(ledComputerPin, OUTPUT);
  servo.write(0);

  if(!rtc.begin()){
    Serial.println("Couldn't find RTC");
    while(1);
  }

  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  
  Serial.println("Approximate your card to the reader...");
  Serial.println();
  lcd.print("Welcome to the");
  lcd.setCursor(2,1);
  lcd.print("smart building");
  delay(5000);
  lcd.clear();
  lcd.print("Scan your badge !");
}
void loop() 
{
  now = rtc.now();
  actualHour = now.hour();
  actualMinute = now.minute();
  
  while(BTserial.available()){
    dataHouse = BTserial.readString();
    if(dataHouse == "1"){
      Serial.println("Coucou");
      //arrivalTime = rtc.now() + 1800; +30minutes
      arrivalTime = rtc.now() + 60;
      Serial.println(arrivalTime.hour());
      Serial.println(arrivalTime.minute());
    }else{
      Serial.println("Alarm set at :");
      Serial.println(dataHouse);
    }
  }

  if(actualHour == arrivalTime.hour() && actualMinute == arrivalTime.minute() && heaterTriggered == 0){
    heaterTriggered = 1;
    turnOnLed();
  }
  lcd.clear();
  lcd.print("Scan your badge !");
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  if (content.substring(1) == "1B 8C 52 D3")
  {
    Serial.println("Authorized access");
    if(numberOfScan == 0){
      lcd.clear();
      lcd.print("Gustave is");
      lcd.setCursor(2,1);
      lcd.print("coming !");
    } else if(numberOfScan == 1){
      lcd.clear();
      lcd.print("Gustave is");
      lcd.setCursor(2,1);
      lcd.print("leaving !");
    }
    Serial.println();
    servo.write(0);
    delay(1000);
    servo.write(90);
    delay(7000);
    servo.write(0);

    if(numberOfScan == 0){
     lcd.clear();
     lcd.print("Welcome back !");
     digitalWrite(led, HIGH);
     delay(500);
     digitalWrite(ledComputerPin, HIGH);
     numberOfScan = 1;
     heaterTriggered = 0;
    }
    else if(numberOfScan == 1){
      lcd.clear();
      lcd.print("Goodbye");
      lcd.setCursor(2,1);
      lcd.print("Gustave !");
      digitalWrite(led, LOW);
      digitalWrite(ledComputerPin, LOW);
      digitalWrite(ledHeaterPin, LOW);
      numberOfScan = 0;
    }
    
    delay(3000);
    lcd.clear();
  }
 
 else   {
    Serial.println(" Access denied");
    lcd.clear();
    lcd.print("You are not allowed");
    lcd.setCursor(2,1);
    lcd.print("to enter !");
    delay(3000);
  }
}

void turnOnLed(){
  digitalWrite(ledHeaterPin, HIGH);
}
