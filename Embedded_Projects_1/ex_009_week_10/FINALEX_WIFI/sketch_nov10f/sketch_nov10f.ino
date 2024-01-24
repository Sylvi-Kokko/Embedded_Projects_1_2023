#include <LiquidCrystal.h>
#include <Wire.h>
#define CMPS14_address 0x60
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
int val;
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

int left_turn(int cm){
  digitalWrite(Motor_L_dir_pin, Motor_return);
  digitalWrite(Motor_R_dir_pin, Motor_forward);
  while(left_count < (cm*1.4)){
    analogWrite(Motor_L_pwm_pin,1000);
    analogWrite(Motor_R_pwm_pin,1000);
  }
  analogWrite(Motor_L_pwm_pin,0);
  analogWrite(Motor_R_pwm_pin,0);
  count_reset();
  return 0;
}

int right_turn(int cm){
  digitalWrite(Motor_R_dir_pin, Motor_return);
  digitalWrite(Motor_L_dir_pin, Motor_forward);
  while(right_count < (cm*1.4)){
    analogWrite(Motor_R_pwm_pin,1000);
    analogWrite(Motor_L_pwm_pin,1000);
  }
  analogWrite(Motor_R_pwm_pin,0);
  analogWrite(Motor_L_pwm_pin,0);
  count_reset();
  return 0;
}

int go_straight(int cm){
  digitalWrite(Motor_L_dir_pin, Motor_forward);
  digitalWrite(Motor_R_dir_pin, Motor_forward);
  while(left_count < (cm*14)){
    analogWrite(Motor_L_pwm_pin,255);
    analogWrite(Motor_R_pwm_pin,255);
  }
  analogWrite(Motor_L_pwm_pin,0);
  analogWrite(Motor_R_pwm_pin,0);
  count_reset();
  return 0;
}
int go_back(int cm){
  digitalWrite(Motor_L_dir_pin, Motor_return);
  digitalWrite(Motor_R_dir_pin, Motor_return);
  while(left_count < (cm*14)){
    analogWrite(Motor_L_pwm_pin,255);
    analogWrite(Motor_R_pwm_pin,255);
  }
  analogWrite(Motor_L_pwm_pin,0);
  analogWrite(Motor_R_pwm_pin,0);
  count_reset();
  return 0;
}

int turn_until(float target){
  
  float degree = 361.0;
  Wire.requestFrom(CMPS14_address, 1, true);
    Wire.beginTransmission(CMPS14_address);
    Wire.write(1);
    Wire.endTransmission(false);
  if (Wire.available() >= 1)
  {
    byte raw = Wire.read();
    degree = 0;
    degree = raw*1.41;
  }else{
    Serial.println("Wire not foundddd");
    return 404;
  }

  int angleDifference = target - degree;
  if (angleDifference > 180) {
    angleDifference -= 360;
  } else if (angleDifference < -180) {
    angleDifference += 360;
  }

  if(angleDifference > 0){
    digitalWrite(Motor_L_dir_pin, Motor_return);
  digitalWrite(Motor_R_dir_pin, Motor_forward);
  }else{
   digitalWrite(Motor_L_dir_pin, Motor_forward);
  digitalWrite(Motor_R_dir_pin, Motor_return);
  }

  while(!(degree <= target + 1 && degree >= target - 1)){
    Serial.println(target);
    Serial.println(degree);
    analogWrite(Motor_L_pwm_pin,80);
    analogWrite(Motor_R_pwm_pin,80);
    Wire.requestFrom(CMPS14_address, 1, true);
    Wire.beginTransmission(CMPS14_address);
    Wire.write(1);
    Wire.endTransmission(false);
  if (Wire.available() >= 1)
  {
    byte raw = Wire.read();
    degree = 0.0;
    degree = raw*1.41592;
  }else{
    Serial.println("Wire not foundddd");
    return 404;
  }
  lcd.print(degree);
  lcd.setCursor(0, 1);
  lcd.print(target);
  }
  analogWrite(Motor_L_pwm_pin,0);
  analogWrite(Motor_R_pwm_pin,0);
  count_reset();
  return 0;
}

void setup() {
  Wire.begin();
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
  if (isOn){
  
    /////Compass//////////////////////
    val = 0;
    lcd.setCursor(0, 0);
    
    //////////////////////

     if (Serial.available() > 0){
    String message = Serial.readStringUntil('\n');
    Serial.print("Message received, content: ");
    Serial.println(message);
    int pos_s;
    int poz_z;
    int movement = message.indexOf("Move");
    int turn = message.indexOf("Turn");
    int until = message.indexOf("UNTIL");
    if (movement > -1){
      Serial.println("Command = movement ");
      pos_s = message.indexOf(":");
      if (pos_s > -1){
        String stat = message.substring(pos_s + 1);
        val = stat.toInt();
        if(val<0){
          go_back((-1)*val);
        }
          lcd.setCursor(0, 0);
          lcd.clear();
          lcd.print(val);
          go_straight(val);
      }
    }
    else if (turn > -1){
      Serial.println("Command = TURN ");
      pos_s = message.indexOf(":");
      if (pos_s > -1){
        String stat = message.substring(pos_s + 1);
        val = stat.toInt();
        if(val < 0){
          right_turn((-1)*val);
        }
          lcd.setCursor(0, 0);
          lcd.clear();
          lcd.print(stat);
          left_turn(val);
      }
    }else if (until > -1){
      Serial.println("Command = UNTIL ");
      pos_s = message.indexOf(":");
      if (pos_s > -1){
        String stat = message.substring(pos_s + 1);
        val = stat.toInt();
        turn_until(val);
        lcd.setCursor(0, 0);
        lcd.clear();
        lcd.print(val);
        
      }
    }
    else{
      Serial.println("No greeting found, try typing Print:Hi or Print:Hello\n");
    }

    
  }
  else{
    analogWrite(Motor_L_pwm_pin,Motor_forward);
    analogWrite(Motor_R_pwm_pin,Motor_forward);
    analogWrite(Motor_L_pwm_pin,0);
    analogWrite(Motor_R_pwm_pin,0);
  }           
  delay(30);
  }
  delay(30);
  lcd.clear();
}