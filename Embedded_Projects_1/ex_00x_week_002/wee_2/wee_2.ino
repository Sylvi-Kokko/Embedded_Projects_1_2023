#include <LiquidCrystal.h>


const int rs = 52, en = 53, d4 = 50, d5 = 51, d6 = 49, d7 = 48;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
const int sw = 13;
const uint8_t vrx = A1, vry = A0;
const long interval = 1000; 
unsigned long previousMillis = 0;
unsigned long lasttimepressed = 0;
bool pressbutton = false;
unsigned long bouncedelay = 200;
int ledState = 0;
float x = 0, y = 0;
const byte button = 2;


void buttonPress(){
  if((millis()-lasttimepressed)> bouncedelay){
    pressbutton = true;
    lasttimepressed = millis();
    }
  }



void setup() {
  // put your setup code here, to run once:
  pinMode(button, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(button), buttonPress, FALLING);
  Serial.begin(9600);
  lcd.begin(20, 4);
}

void loop() {
  lcd.clear();
  x = analogRead(vrx);
  int col = ((x / 822.0)*19);
  lcd.setCursor(4, 0);
  lcd.print("X-position");
  lcd.setCursor(0, 1);
  lcd.print("-100%....0%.....100%");
  lcd.setCursor(col, 3);
  lcd.print("|");
  if (pressbutton) {
    Serial.println("Pressed");  
    pressbutton = false;
  }
  delay(30);
  }
