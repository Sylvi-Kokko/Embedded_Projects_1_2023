#include <LiquidCrystal.h>


const int rs = 52, en = 53, d4 = 50, d5 = 51, d6 = 49, d7 = 48;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
const int sw = 13;
const uint8_t vrx = A0, vry = A1;
const long interval = 1000; 
unsigned long previousMillis = 0;
int ledState = 0;
float x = 0, y = 0;

ISR(){
  lcd.clear();
  lcd.setCursor(7,1);
  lcd.printl("Boo!");
}

attatchInterrupt(2, ISR, FALLING);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  lcd.begin(20, 4);
}

void loop() {
  x = analogRead(vrx);
  y = analogRead(vry);
  lcd.setCursor(0,1);
  lcd.println((x/822.0)*5);
  lcd.setCursor(0, 2);
  lcd.println((y/822.0)*5);
  delay(1000);
  lcd.clear();
  }
