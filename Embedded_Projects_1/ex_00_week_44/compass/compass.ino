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
void l_rising(){
  left_count +=1;
}
void r_rising(){
  right_count +=1;
}
void setup() {
  pinMode(Encoder_INT4, INPUT);
  pinMode(Encoder_Int5, INPUT);
  Wire.begin();
  pinMode(buttonPin, INPUT);
  Serial.begin(9600);
  lcd.begin(20, 4);
}
void loop() {
  lcd.clear();
  float lol;
  lcd.setCursor(0, 0);
    Wire.requestFrom(CMPS14_address, 1, true);
    Wire.beginTransmission(CMPS14_address);
    Wire.write(1);
    Wire.endTransmission(false);
    if (Wire.available() >= 1)
    {
      byte raw = Wire.read();
      lol = 0;
      lol = raw*1.41;
    }
if(lol >= 242.0 && lol < 287.0){
lcd.print("N");
}else if(lol>=287.0 && lol < 332.0){
lcd.print("NE");
}else if(lol >= 332.0 || (lol >= 0.0 && lol < 17.0)){
lcd.print("E");
}else if(lol>=17.0 && lol < 63.0){
lcd.print("SE");
}else if(lol>63.0 && lol < 107.0){
lcd.print("S");
}else if(lol >= 107.0 && lol < 152.0){
lcd.print("SW");
}else if(lol >= 152.0 && lol < 197.0){
lcd.print("W");
}else if(lol >= 197.0 && lol < 242.0){
lcd.print("NW");
}
      lcd.setCursor(0, 1);
      lcd.println(lol);
      Serial.println(lol);
    delay(100);
  }