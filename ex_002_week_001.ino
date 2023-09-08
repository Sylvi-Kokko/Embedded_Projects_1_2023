#include <LiquidCrystal.h>


const int rs = 52, en = 53, d4 = 50, d5 = 51, d6 = 49, d7 = 48;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


const int red1 = 3;
const int yellow1 = 4;
const int green1 = 5;
const int red2 = 6;
const int yellow2 = 7;
const int green2 = 8;

const long interval = 1000; 
unsigned long previousMillis = 0;
int ledState = 0;

void setup() {
  // put your setup code here, to run once:

  lcd.begin(20, 4);

pinMode(red1, OUTPUT);
pinMode(red2, OUTPUT);
pinMode(yellow1, OUTPUT);
pinMode(yellow2, OUTPUT);
pinMode(green1, OUTPUT);
pinMode(green2, OUTPUT);

digitalWrite(red1, HIGH);
digitalWrite(red2, HIGH);
digitalWrite(yellow1, HIGH);
digitalWrite(yellow2, HIGH);
digitalWrite(green1, HIGH);
digitalWrite(green2, HIGH);



}

void loop() {
  // here is where you'd put code that needs to be running all the time.

  // check to see if it's time to blink the LED; that is, if the difference
  // between the current time and last time you blinked the LED is bigger than
  // the interval at which you want to blink the LED.
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    lcd.clear();
    // save the last time you blinked the LED
    previousMillis = currentMillis;

  digitalWrite(red1, LOW);
  digitalWrite(red2, LOW);
  digitalWrite(yellow1, LOW);
  digitalWrite(yellow2, LOW);
  digitalWrite(green1, LOW);
  digitalWrite(green2, LOW);

    // if the LED is off turn it on and vice-versa:
    if (ledState == 0 || ledState == 1) {
      digitalWrite(red1, HIGH);

      digitalWrite(green2, HIGH);
    } else if (ledState == 2) {
      digitalWrite(red1, HIGH);
      digitalWrite(yellow1, HIGH);

      digitalWrite(yellow2, HIGH);
    }
    else if (ledState == 3 || ledState == 4) {
      digitalWrite(green1, HIGH);

      digitalWrite(red2, HIGH);
    }
    else if (ledState == 5) {
      digitalWrite(yellow1, HIGH);

      digitalWrite(red2, HIGH);
      digitalWrite(yellow2, HIGH);
    }
    

    ledState++;
    if (ledState == 6) {
      ledState = 0;
    }  
    lcd.setCursor(0, 0);
    lcd.print("Pot.meter = ");
    lcd.print((analogRead(A1) / 1023.0) * 5.0);
    lcd.print(" V");
    
    lcd.setCursor(0, 1);
    lcd.print("3.3 level = ");
    lcd.print((analogRead(A2)/1023.0) * 5.0);
    lcd.print(" V");


    

    



  }
}
