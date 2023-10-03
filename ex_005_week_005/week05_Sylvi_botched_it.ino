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
int speedx, speedy;
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
    val1 = analogRead(analogPin1);  // read the input pin
    val2 = analogRead(analogPin2);
    speedx = map(val1, 0, 1023, -255, 255);
    speedy = map(val2, 0, 1023, -255, 255);
    if(speedy > 520){
      analogWrite(Motor_L_pwm_pin,Motor_forward);
      analogWrite(Motor_L_pwm_pin, speedy);
    }
    else if(speedy < 480){
      analogWrite(Motor_L_pwm_pin,Motor_return);
      analogWrite(Motor_L_pwm_pin, speedy);      
    }
    else{
      analogWrite(Motor_L_pwm_pin, 0);
    }
    if (speedx > 520){
      analogWrite(Motor_R_pwm_pin,Motor_forward);
      analogWrite(Motor_R_pwm_pin, speedx);      
    }
    else if(speedx < 480){
      analogWrite(Motor_R_pwm_pin, Motor_return);
      analogWrite(Motor_R_pwm_pin, speedx);      
    }
    else{
      analogWrite(Motor_R_pwm_pin, 0);
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
    val1 = 0;   
    val2 = 0;  
  }
  else{
    analogWrite(Motor_L_pwm_pin,Motor_forward);
    analogWrite(Motor_R_pwm_pin,Motor_forward);
    analogWrite(Motor_L_pwm_pin,0);
    analogWrite(Motor_R_pwm_pin,0);
  }           
  delay(30);
  }