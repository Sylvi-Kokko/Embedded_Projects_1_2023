#include <LiquidCrystal.h>
#include <Math.h>
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
int y_akseli, x_pwm, pwm_R, pwm_L, dir_L, dir_R, joystick_y, joystick_x;
float x_akseli, val1, val2;
volatile int left_count = 0;
volatile int right_count = 0;
float joy_x, joy_y;

const byte buttonPin = 19;
bool isOn = false;

void interrut(){
  isOn = !isOn;
}
void l_rising(){
  left_count +=1;
}
void r_rising(){
  right_count +=1;
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
    joystick_y = analogRead(analogPin2);
    joystick_x = analogRead(analogPin1);
    
    // -1 to 1
    joy_y = ((float)joystick_y - 503.0f) / 503.0f;
    joy_x = ((float)joystick_x - 496.0f) / 496.0f;

    auto motor_1 = -joy_x + joy_y;
    auto motor_2 =  joy_x + joy_y;
    
    digitalWrite(Motor_L_pwm_pin, abs( motor_1 * 500.0f));    
    digitalWrite(Motor_R_pwm_pin, abs( motor_2 * 500.0f));    
    digitalWrite(Motor_L_dir_pin, motor_1 < 0.0f);
    digitalWrite(Motor_R_dir_pin, motor_2 < 0.0f);
    delay(30); 
  }
  else{
    analogWrite(Motor_L_pwm_pin,0);
    analogWrite(Motor_R_pwm_pin,0);
    lcd.print(isOn);  
  }           
  lcd.setCursor(0, 0);
  lcd.print("Left count is: ");
  lcd.print(left_count);
  lcd.setCursor(0,1);
  lcd.print("Right count is: ");
  lcd.print(right_count);
  lcd.setCursor(0, 2);
  lcd.print("The joy_x is: ");
  lcd.print(joystick_x);
  lcd.setCursor(0,3);
  lcd.print("The joy_y is: ");
  lcd.print(joystick_y);
  delay(30);
  }