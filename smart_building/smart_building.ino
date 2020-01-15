#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>
 
#define SS_PIN 10
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN);
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);
SoftwareSerial BTserial(0,1);

int led = A0;
int servoPin = 8;
int numberOfScan = 0;

String dataHouse;
String data = "";

Servo servo;
 
void setup() 
{
  Serial.begin(9600);
  BTserial.begin(38400);
  SPI.begin();
  lcd.begin(16,2);
  
  mfrc522.PCD_Init();
  servo.attach(servoPin);

  pinMode(led, OUTPUT);
  servo.write(0);
  
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
  while(BTserial.available()){
    dataHouse = BTserial.readString();
    Serial.println("DATA FROM HOUSE :");
    Serial.println(dataHouse);
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
     numberOfScan = 1;
    }
    else if(numberOfScan == 1){
      lcd.clear();
      lcd.print("Have a good evening");
      lcd.setCursor(2,1);
      lcd.print("Gustave !");
      digitalWrite(led, LOW);
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
