#include <LiquidCrystal.h>
#define Motor_forward         0
#define Motor_return          1
#define Motor_L_dir_pin       7
#define Motor_R_dir_pin       8
#define Motor_L_pwm_pin       9
#define Motor_R_pwm_pin       10

const int rs = 52, en = 53, d4 = 50, d5 = 51, d6 = 49, d7 = 48;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
int ledPin1 = 44;      // LED connected to digital pin 44
int ledPin2 = 45;
int analogPin1 = A0;   // potentiometer connected to analog pin 0
int analogPin2 = A1;
int val1 = 0;  
int val2 = 0;
int st_Y = 496;
int st_X = 503;
int pwm_R = 0;
int pwm_L = 0;

const byte buttonPin = 19;
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
  
  pinMode(buttonPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(19), interrut, FALLING);
  Serial.begin(9600);
  lcd.begin(20, 4);
}

void loop() {
  lcd.clear();
  if (isOn){
    val1 = analogRead(analogPin1);  // read the input pin 0 - 1023
    val2 = analogRead(analogPin2); 
    if ((st_Y - val1) > 5){ //If value from starting - current is positive, move backwards
      digitalWrite(Motor_R_dir_pin, Motor_return);
      pwm_R = 496 - val1;
    }
    else if((st_Y - val1) < -5){ //If value from starting - current is negative move backwards
      digitalWrite(Motor_R_dir_pin, Motor_forward);
      pwm_R = val1; //if reading is 0 go full speed
    }
    else {
      pwm_R = 0;
    }
    if ((st_X - val2) > 5){ //
      digitalWrite(Motor_L_dir_pin, Motor_return); 
      pwm_L = 503 - val2;
    }
    else if((st_X - val2) < -5){
      digitalWrite(Motor_L_dir_pin, Motor_forward);
      pwm_L = val2;
    }
    else {
      pwm_L = 0;
    }
    analogWrite(Motor_L_pwm_pin,pwm_L);
    analogWrite(Motor_R_pwm_pin,pwm_R);
    lcd.setCursor(0, 0);
    lcd.print("Value 1 is: ");
    lcd.print(pwm_R);
    lcd.print(" and");
    lcd.setCursor(0,1);
    lcd.print("Value 2 is: ");
    lcd.print(pwm_L);
    lcd.setCursor(0,2);
    lcd.print(isOn);  
    delay(30); 

  }
  else{
    analogWrite(Motor_L_pwm_pin,Motor_forward);
    analogWrite(Motor_R_pwm_pin,Motor_forward);
    analogWrite(Motor_L_pwm_pin,0);
    analogWrite(Motor_R_pwm_pin,0);
  }           
  delay(30);
  }