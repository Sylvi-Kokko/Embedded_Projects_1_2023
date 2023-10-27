#include <LiquidCrystal.h>
#include <Wire.h>
#define Motor_forward         0
#define Motor_return          1
#define Motor_L_dir_pin       7
#define Motor_R_dir_pin       8
#define Motor_L_pwm_pin       9
#define Motor_R_pwm_pin       10
#define Encoder_PA1           23
#define Encoder_PA2           24
#define Encoder_Int5          3
#define Encoder_INT4          2

const int rs = 52, en = 53, d4 = 50, d5 = 51, d6 = 49, d7 = 48;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
int analogPin1 = A0;   // potentiometer connected to analog pin 0
int analogPin2 = A1;
int val1, val2, pwm_R, pwm_L;  
int st_Y = 503;
int st_X = 496;
int left_count = 0;
int right_count = 0;

const byte buttonPin = 19;
bool isOn = true;

void interrut(){
  if (isOn){
    isOn = false;
  }
  else if (!isOn){
    isOn = true;
  }
}

void count_reset() {
  left_count = 0;
  right_count = 0;
}

void l_rising(){
  left_count +=1;
}
void r_rising(){
  right_count +=1;
}

void left_turn(cm){
  digitalWrite(Motor_L_dir_pin, Motor_return);
  while(left_count < (cm*14)){
    analogWrite(Motor_L_pwm_pin,1000);
  }
  analogWrite(Motor_L_pwm_pin,0);
  count_reset();
}
void right_turn(cm){
  digitalWrite(Motor_R_dir_pin, Motor_return);
  while(right_count < (cm*14)){
    analogWrite(Motor_R_pwm_pin,1000);
  }
  analogWrite(Motor_R_pwm_pin,0);
  count_reset();
}

void go_straight(cm){
  digitalWrite(Motor_L_dir_pin, Motor_forward);
  digitalWrite(Motor_R_dir_pin, Motor_forward);
  while(left_count < (cm*14)){
    analogWrite(Motor_L_pwm_pin,1000);
    analogWrite(Motor_L_pwm_pin,1000);
  }
  analogWrite(Motor_L_pwm_pin,0);
  analogWrite(Motor_R_pwm_pin,0);
  count_reset();
}
void go_back(cm){
  digitalWrite(Motor_L_dir_pin, Motor_return);
  digitalWrite(Motor_R_dir_pin, Motor_return);
  while(left_count < (cm*14)){
    analogWrite(Motor_L_pwm_pin,1000);
    analogWrite(Motor_L_pwm_pin,1000);
  }
  analogWrite(Motor_L_pwm_pin,0);
  analogWrite(Motor_R_pwm_pin,0);
  count_reset();
}

void setup() {
  
  pinMode(buttonPin, INPUT);
  pinMode(Encoder_INT4, INPUT);  
  pinMode(Encoder_Int5, INPUT);
  attachInterrupt(digitalPinToInterrupt(19), interrut, FALLING);
  attachInterrupt(digitalPinToInterrupt(2), r_rising, RISING);
  attachInterrupt(digitalPinToInterrupt(3), l_rising, RISING);
  Serial.begin(9600);
  lcd.begin(20, 4);
}

void loop() {
  lcd.clear();
  if (isOn){

    //Hard coded maze
    go_straight(50);
    left_turn(19);
    go_straight(50);
    //////////////////////

    analogWrite(Motor_L_pwm_pin,0);
    analogWrite(Motor_R_pwm_pin,0);

    //LCD //////////////////////////////////
    lcd.setCursor(0, 0);
    lcd.print("Encoder B right: ");
    lcd.print(right_count);
    lcd.setCursor(0,1);
    lcd.print("Encoder B left: ");
    lcd.print(digitalRead(Encoder_PA2));
    lcd.setCursor(0,2);
    lcd.print("Encoder A right: ");
    lcd.print(left_count);
    lcd.setCursor(1,3);
    lcd.print("Encoder A left: ");
    lcd.print(digitalRead(Encoder_PA1));
    lcd.setCursor(0,4);     
    //////////////////////////////////////////
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