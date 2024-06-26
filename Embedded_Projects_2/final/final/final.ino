
/**
* @file Ex001_Week0.ino
* @authors Dordze Ostrowski, Sylvi Kokko, Wilhelm Nilsson
* @brief Project for Embedded Projects course (TAMK, Spring 2024)
* @date 2024-16-04
*/

// Website sends the following text to serial monitor once button is pressed "start_program"

#include <LiquidCrystal.h>
#include <Wire.h>
#include "LIDARLite_v4LED.h"
#include <EEPROM.h>
#include <DFRobot_TCS34725.h>

#define CMPS14_address 0x60
#define Motor_forward         1
#define Motor_return          0
#define Motor_L_dir_pin       7
#define Motor_R_dir_pin       8
#define Motor_L_pwm_pin       9
#define Motor_R_pwm_pin       10
#define Encoder_PA1           23
#define Encoder_PA2           24
#define Encoder_Int5          3
#define Encoder_Int4          2
#define MAX_READINGS 10

const int rs = 52, en = 53, d4 = 50, d5 = 51, d6 = 49, d7 = 48;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
int analogPin1 = A0;
int analogPin2 = A1;
LIDARLite_v4LED myLIDAR;
const byte buttonPin = 19;
int left_count = 0, right_count = 0, presses = 0, currentIndex = 0;
int heading = 0;
int enginePower = 180;
int backWall=308, rightWall=55, leftWall=233;
bool start = false, wallHunt = false;
int wDistB=15, wDistR=10, wDistL=100;
float lidAv;
int target = -1;
int pass = -1;
struct match{
  int dif;
  int color;
};
struct encoder_lidar{
  int lidarDistance;
  int encoderDistance;
};
struct lidar_comp{
  float lidarDistance;
  int compassDegree;
};
struct RGB {
  int r;
  int g;
  int b;
};
RGB colors[4]; //0 = red, 1 = blue, 2 = green, 3 = yellow (Goal)
int colDifTreshold = 50;
DFRobot_TCS34725 tcs = DFRobot_TCS34725(&Wire, TCS34725_ADDRESS,TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
typedef struct RGB co;
RGB currentColor = {5,5,5};
match cuCol = {500,5};


void LidarAvg() {
  int LidarVals[MAX_READINGS];
  static bool arrayFilled = false;

  currentIndex++;
  if (currentIndex >= MAX_READINGS) {
      currentIndex = 0;
      arrayFilled = true;
  }
  int numReadings = arrayFilled ? MAX_READINGS : currentIndex;
  int olddist = myLIDAR.getDistance();
for (int i = 0; i < numReadings; i++) {
    count_reset();
    digitalWrite(Motor_L_dir_pin, Motor_forward);
    digitalWrite(Motor_R_dir_pin, Motor_forward);
    analogWrite(Motor_L_pwm_pin,170);
    analogWrite(Motor_R_pwm_pin,170);
    if(right_count >= 14){
      analogWrite(Motor_L_pwm_pin,0);
      analogWrite(Motor_R_pwm_pin,0);
    }
    int newdist = myLIDAR.getDistance();
    LidarVals[currentIndex] = (olddist-newdist)/right_count;
    olddist = newdist;
  }
 //Calculate average from LidarVals and return it
 int factor = 0;
  for(int i=0; i<MAX_READINGS; i++) {
    factor += LidarVals[i];
  }
    lidAv = factor/MAX_READINGS;
    Serial2.println("Encoder=" + String(lidAv));
}

void buttonPressed(){
  analogWrite(Motor_L_pwm_pin,0);
  analogWrite(Motor_R_pwm_pin,0);
}
void count_reset() { //Count reset is called so that every new motion can start from 0
  left_count = 0;
  right_count = 0;
}
void l_rising(){
  left_count += 1;
}
void r_rising(){
  right_count +=1;
}
int wiregetdegree(){
  int lcddegree = 0;
  Wire.requestFrom(CMPS14_address, 1, true);
    Wire.beginTransmission(CMPS14_address);
    Wire.write(1);
    Wire.endTransmission(false);
  if (Wire.available() >= 1)
  {
    byte raw = Wire.read();
    lcddegree = 0.0;
    lcddegree = raw*1.41592 + 161.05;
    lcddegree = lcddegree % 360;
    if (lcddegree < 0) {
    lcddegree += 360;
    }
  }else{
    Serial.println("Wire not foundddd");
    return 404;
  }
  return lcddegree;
}
void measure(){
  struct lidar_comp readings[18];
  int current = wiregetdegree();
  int target = 0;
  for(int i=0; i<18; i++){
    current = wiregetdegree();
    target = wiregetdegree() + 20;
    if(target > 360){
      target = target - 360;
    }
    digitalWrite(Motor_L_dir_pin, Motor_forward);
    digitalWrite(Motor_R_dir_pin, Motor_return);
    analogWrite(Motor_L_pwm_pin,50);
    analogWrite(Motor_R_pwm_pin,50);
    if(current == target){
      analogWrite(Motor_L_pwm_pin,0);
      analogWrite(Motor_R_pwm_pin,0);
    }
    readings[i].compassDegree = wiregetdegree();
    readings[i].lidarDistance = myLIDAR.readDistance();
  }
}

void Obstacle() {
  analogWrite(Motor_L_pwm_pin, 0);
  analogWrite(Motor_R_pwm_pin, 0);

  int current = wiregetdegree();
  if (pass == -1) {
    Serial.println("passing");
    digitalWrite(Motor_L_dir_pin, Motor_return);
    digitalWrite(Motor_R_dir_pin, Motor_return);
    analogWrite(Motor_L_pwm_pin, enginePower);
    analogWrite(Motor_R_pwm_pin, enginePower);
    delay(1000);
    if (target == -1) {
      target = wiregetdegree() - 75;
      if (target < 0) {
        target += 360;
      }
    }

    digitalWrite(Motor_L_dir_pin, Motor_return);
    digitalWrite(Motor_R_dir_pin, Motor_forward);
    analogWrite(Motor_L_pwm_pin, enginePower);
    analogWrite(Motor_R_pwm_pin, enginePower);

    while (true) {
      current = wiregetdegree();
      Serial.println(current);
      if (current <= target + 10 && current >= target - 10) {
        digitalWrite(Motor_L_dir_pin, Motor_forward);
        digitalWrite(Motor_R_dir_pin, Motor_forward);
        analogWrite(Motor_L_pwm_pin, 0);
        analogWrite(Motor_R_pwm_pin, 0);
        break;
      }
      pass = 1;
      target = -1;
      return;
    }

    // Turn final
    if (target == -1) {
      target = wiregetdegree() + 90;
      if (target > 360) {
        target -= 360;
      }
    }
    digitalWrite(Motor_L_dir_pin, Motor_return);
    digitalWrite(Motor_R_dir_pin, Motor_return);
    analogWrite(Motor_L_pwm_pin, enginePower);
    analogWrite(Motor_R_pwm_pin, enginePower);
    delay(1000);
    digitalWrite(Motor_L_dir_pin, Motor_forward);
    digitalWrite(Motor_R_dir_pin, Motor_return);
    analogWrite(Motor_L_pwm_pin, enginePower);
    analogWrite(Motor_R_pwm_pin, enginePower);

    while (true) {
      current = wiregetdegree();
      if (current <= target + 10 && current >= target - 10) {
        digitalWrite(Motor_L_dir_pin, Motor_forward);
        digitalWrite(Motor_R_dir_pin, Motor_forward);
        analogWrite(Motor_L_pwm_pin, 0);
        analogWrite(Motor_R_pwm_pin, 0);
        break;
      }
    }
    target = -1;
    pass = -1;
    return;
  }
}


void calcColDif(){
  RGB c = RGBsensor();
  int colorDif;
  match colMatch{500,5};
  for(int i = 0; i < 4; i++){
    int r_diff = c.r - colors[i].r;
    int g_diff = c.g - colors[i].g;
    int b_diff = c.b - colors[i].b;
    colorDif = abs(r_diff) + abs(g_diff) + abs(b_diff);
    if(colorDif < colMatch.dif){
      colMatch.dif = colorDif;
      colMatch.color = i;
    }
  }
  cuCol = colMatch;
}

struct RGB RGBsensor(){
  uint16_t clear, red, green, blue;
  tcs.getRGBC(&red, &green, &blue, &clear);
  tcs.lock();
  uint32_t sum = clear;
  float r, g, b;
  r = red;
  r /= sum;
  g = green;
  g /= sum;
  b = blue;
  b /= sum;
  r *= 256;
  g *= 256;
  b *= 256;
  struct RGB col;
  col.r = r;
  col.g = g;
  col.b = b;
  return col;
}

void setup() {
  Wire.begin();
  pinMode(buttonPin, INPUT);
  pinMode(Encoder_Int4, INPUT);
  pinMode(Encoder_Int5, INPUT);
  attachInterrupt(digitalPinToInterrupt(19), buttonPressed, FALLING); //Detects button being pressed
  attachInterrupt(digitalPinToInterrupt(2), r_rising, RISING); //Detects right wheel rotation
  attachInterrupt(digitalPinToInterrupt(3), l_rising, RISING); //Detects left wheel rotation
  Serial.begin(9600);
  Serial2.begin(9600);
  lcd.begin(20, 4);
  if (myLIDAR.begin() == false) {
    Serial.println("Device did not acknowledge! Freezing.");
    delay(1000);
  }
  analogWrite(Motor_L_pwm_pin,0);
  analogWrite(Motor_R_pwm_pin,0);
  Serial.println("LIDAR acknowledged!");
  while(!tcs.begin()){
    Serial.println("No TCS34725 found ... check your connections");
    delay(1000);
  }
  for(int i = 0; i < 4; i++){
    while(digitalRead(19) == HIGH){}
    colors[i] = RGBsensor();
    Serial.println("color recorded");
    delay(200);
  }
}

void loop() {
// while loop to wait until start button is pressed on website, and won't check again after starting.
  while(start == false){
    analogWrite(Motor_L_pwm_pin,0);
    analogWrite(Motor_R_pwm_pin,0);
    String message = Serial.readStringUntil('\n');
    if (message=="start"){
      Serial.println("starting program");
      start = !start;
    }
}
 //Return the preset it's the most similar to
calcColDif();  
int lid = myLIDAR.getDistance();
 Serial.println(cuCol.color);
  Serial.println(cuCol.dif);
if(cuCol.color == 0 && cuCol.dif < colDifTreshold) {
  Serial.println("Red detected");
  digitalWrite(Motor_L_dir_pin, Motor_return);
  digitalWrite(Motor_R_dir_pin, Motor_return);
  analogWrite(Motor_L_pwm_pin, 200);
  analogWrite(Motor_R_pwm_pin, 200);
  delay(1000);
    //stop
  analogWrite(Motor_L_pwm_pin,0);
  analogWrite(Motor_R_pwm_pin,0);
  int current = wiregetdegree();
  //turn
  if(target == -1) {int target = wiregetdegree()+90;}
  if(target > 360){
      target = target - 360;
    }
  digitalWrite(Motor_L_dir_pin, Motor_forward);
  digitalWrite(Motor_R_dir_pin, Motor_return);
  analogWrite(Motor_L_pwm_pin, enginePower);
  analogWrite(Motor_R_pwm_pin, enginePower);
  while(true){
  current = wiregetdegree();
  Serial.println(wiregetdegree());
  if(current <= target+6 && current >= target-6) {
  digitalWrite(Motor_L_dir_pin, Motor_forward);
  digitalWrite(Motor_R_dir_pin, Motor_forward);
  analogWrite(Motor_L_pwm_pin,0);
  analogWrite(Motor_R_pwm_pin,0);
  target=-1;
  break;
  }  
}
}
if(lid <= 12) {
  Serial.print("Obstacle detected");
  Obstacle();
}
if(cuCol.color == 1 && cuCol.dif < colDifTreshold) {
  Serial.print("Blue detected");
  enginePower = 100;
}
else if(cuCol.color == 2 && cuCol.dif < colDifTreshold) { //Detect green
  Serial.print("Yellow detected");
  enginePower = 120;
  wallHunt = true;
}
else if(cuCol.color == 3 && cuCol.dif < colDifTreshold - 38) {
  Serial.print("Goal detected");
  digitalWrite(Motor_L_dir_pin, Motor_forward);
  digitalWrite(Motor_R_dir_pin, Motor_forward);
  analogWrite(Motor_L_pwm_pin,0);
  analogWrite(Motor_R_pwm_pin,0);
  start = false;
}
else {
  digitalWrite(Motor_L_dir_pin, Motor_forward);
  digitalWrite(Motor_R_dir_pin, Motor_forward);
  analogWrite(Motor_L_pwm_pin,enginePower);
  analogWrite(Motor_R_pwm_pin,enginePower);
}
if(wallHunt){
  digitalWrite(Motor_L_dir_pin, Motor_forward);
  digitalWrite(Motor_R_dir_pin, Motor_return);
  analogWrite(Motor_L_pwm_pin,enginePower);
  analogWrite(Motor_R_pwm_pin,enginePower);
  while(true){
  int current = wiregetdegree();
  if(current <= backWall+3 && current >= backWall-3) {
    digitalWrite(Motor_L_dir_pin, Motor_forward);
    digitalWrite(Motor_R_dir_pin, Motor_forward);
    analogWrite(Motor_L_pwm_pin,0);
    analogWrite(Motor_R_pwm_pin,0);
    break;
  }
  digitalWrite(Motor_L_dir_pin, Motor_forward);
  digitalWrite(Motor_R_dir_pin, Motor_forward);
  analogWrite(Motor_L_pwm_pin,120);
  analogWrite(Motor_R_pwm_pin,120);
  while(true){
    if(myLIDAR.getDistance() < wDistB){
      analogWrite(Motor_L_pwm_pin,0);
      analogWrite(Motor_R_pwm_pin,0);
      break;
    }
  }
  calcColDif();
  if (cuCol.color == 3  && cuCol.dif > colDifTreshold){
    return;
  }
  /////////////////////////////////
  digitalWrite(Motor_L_dir_pin, Motor_forward);
  digitalWrite(Motor_R_dir_pin, Motor_return);
  analogWrite(Motor_L_pwm_pin,enginePower);
  analogWrite(Motor_R_pwm_pin,enginePower);
  while(true){
  current = wiregetdegree();
  if(current <= rightWall+3 && current >= rightWall-3) {
    digitalWrite(Motor_L_dir_pin, Motor_forward);
    digitalWrite(Motor_R_dir_pin, Motor_forward);
    analogWrite(Motor_L_pwm_pin,0);
    analogWrite(Motor_R_pwm_pin,0);
    target=-1;
    break;
  }
  digitalWrite(Motor_L_dir_pin, Motor_forward);
  digitalWrite(Motor_R_dir_pin, Motor_forward);
  analogWrite(Motor_L_pwm_pin,120);
  analogWrite(Motor_R_pwm_pin,120);
  while(true){
    if(myLIDAR.getDistance() < wDistR){
      analogWrite(Motor_L_pwm_pin,0);
      analogWrite(Motor_R_pwm_pin,0);
      break;
    }
  }
  calcColDif();
  if (cuCol.color == 3){
    return;
  }
  //////////////////////////////////////////////////
  digitalWrite(Motor_L_dir_pin, Motor_forward);
  digitalWrite(Motor_R_dir_pin, Motor_return);
  analogWrite(Motor_L_pwm_pin,50);
  analogWrite(Motor_R_pwm_pin,50);
  while(true){
  current = wiregetdegree();
  if(current <= leftWall+3 && current >= leftWall-3) {
    digitalWrite(Motor_L_dir_pin, Motor_forward);
    digitalWrite(Motor_R_dir_pin, Motor_forward);
    analogWrite(Motor_L_pwm_pin,0);
    analogWrite(Motor_R_pwm_pin,0);
    target=-1;
    break;
  }
  digitalWrite(Motor_L_dir_pin, Motor_forward);
  digitalWrite(Motor_R_dir_pin, Motor_forward);
  analogWrite(Motor_L_pwm_pin,120);
  analogWrite(Motor_R_pwm_pin,120);
  while(true){
    if(myLIDAR.getDistance() < wDistL){
      analogWrite(Motor_L_pwm_pin,0);
      analogWrite(Motor_R_pwm_pin,0);
      break;
    }
  }
  calcColDif();
  if (cuCol.color == 2){
    return;
  }
  }
}
}
}
}