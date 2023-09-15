#include <LiquidCrystal.h>


const int rs = 52, en = 53, d4 = 50, d5 = 51, d6 = 49, d7 = 48;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
int ledPin1 = 44;      // LED connected to digital pin 44
int ledPin2 = 45;
int analogPin1 = A0;   // potentiometer connected to analog pin 0
int analogPin2 = A1;
int val1 = 0;  
int val2 = 0;
const byte buttonPin = 3;
bool isOn = true;

void interrut(){
    Serial.print("Pressed");
  if (isOn){
    isOn = false;
  }
  else if (!isOn){
    isOn = true;
  }
}

void setup() {
  // put your setup code here, to run once:
  pinMode(buttonPin, INPUT);
  pinMode(ledPin1, OUTPUT);  // sets the pin as output
  pinMode(ledPin2, OUTPUT);  // sets the pin as output

  attachInterrupt(digitalPinToInterrupt(3), interrut, FALLING);
  Serial.begin(9600);
  lcd.begin(20, 4);
}

void loop() {
  lcd.clear();
  if (isOn){
    val1 = analogRead(analogPin1);  // read the input pin
    val2 = analogRead(analogPin2);
    analogWrite(ledPin1, val1 / 4); // analogRead values go from 0 to 1023, analogWrite values from 0 to 255
    analogWrite(ledPin2, val2 / 4);
  }
  else{
    analogWrite(ledPin1, 0);
    analogWrite(ledPin2, 0);
  }

  lcd.setCursor(0, 0);
  lcd.print("Value 1 is: ");
  lcd.print(val1);
  lcd.print(" and");
  lcd.setCursor(0,1);
  lcd.print("Value 2 is: ");
  lcd.print(val2);
  lcd.setCursor(0,2);
  lcd.print(isOn);  
  delay(30);
  }
