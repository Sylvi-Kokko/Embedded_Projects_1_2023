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
int y_akseli, x_pwm, pwm_R, pwm_L, dir_L, dir_R;
float x_akseli, val1, val2;
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
    val1 = analogRead(analogPin2);
    val2 = analogRead(analogPin1);
    val2 = val2 + 15.5;
    val1 = val1 + 9.5;
    Serial.println("Val1 is: ");
    Serial.println(val1);
    Serial.println("Val2 is: ");
    Serial.println(val2);
    x_pwm = map(val2, 0, 1023, -500, 500); //-100 to 100
    val2 = val2/496; //0-2
    x_akseli = map(val2, 0, 2, 0, 1); //0  to  1
    y_akseli = map(val1, 0, 1023, -500, 500); //-100 to 100
    x_pwm = abs(x_pwm); // 100 to 0 to 100
    ////Direction
    dir_R = x_akseli;
    dir_R == 0 ? dir_L = 1 : dir_L = 0;
    if((x_akseli == 0) && (y_akseli < 0)){
      dir_L = 1;
      dir_R = 1; 
    }
    else if ((x_akseli == 0) && (y_akseli > 0)){
      dir_L = 0;
      dir_R = 0; 
    }
    digitalWrite(Motor_L_dir_pin, dir_L);
    digitalWrite(Motor_R_dir_pin, dir_R); 
    pwm_L = x_pwm + abs(y_akseli);
    pwm_R = x_pwm + abs(y_akseli);;
    analogWrite(Motor_L_pwm_pin,pwm_L);
    analogWrite(Motor_R_pwm_pin,pwm_R);
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
  delay(30);
  }