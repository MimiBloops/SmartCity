#include <Wire.h>

int ledheaeterPin = 11;
int x = 0;
int brightness = 0; 
int fadeAmount = 5;

void setup() {
  // put your setup code here, to run once:
  pinMode(ledheaeterPin, OUTPUT);
  Wire.begin(9);
  Wire.onReceive(receiveEvent);
  Serial.begin(9600);
}

void receiveEvent(int bytes){
  x = Wire.read();
  Serial.println("RECEIVE EVENT");
  Serial.println(x);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(x == 1){
    Serial.println("X == 1");
    digitalWrite(ledheaeterPin, brightness);
    brightness = brightness + fadeAmount;
    delay(30);
  }
}
