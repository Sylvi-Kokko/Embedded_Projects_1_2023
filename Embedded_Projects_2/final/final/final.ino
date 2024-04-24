
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
int enginePower = 75;
int backWall, rightWall, leftWall;
bool start = false, wallHunt = false;
int wDistB, wDistR, wDistL;
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
int colDifTreshold = 80;
DFRobot_TCS34725 tcs = DFRobot_TCS34725(&Wire, TCS34725_ADDRESS,TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
typedef struct RGB co;
struct match calcColDif(co *c);


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
    Serial2.println("Encoder=" + String(lidAv))
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

void Obstacle(){
  analogWrite(Motor_L_pwm_pin,0);
  analogWrite(Motor_R_pwm_pin,0);
  int current = wiregetdegree();
if(pass == -1) {
  digitalWrite(Motor_L_dir_pin, Motor_return);
  digitalWrite(Motor_R_dir_pin, Motor_return);
  analogWrite(Motor_L_pwm_pin,enginePower);
  analogWrite(Motor_R_pwm_pin,enginePower);
  delay(100);
  if(target == -1) {int target = wiregetdegree()-45;}
  if(target < 0){
      target = target + 360;
    }
  digitalWrite(Motor_L_dir_pin, Motor_return);
  digitalWrite(Motor_R_dir_pin, Motor_forward);
  analogWrite(Motor_L_pwm_pin,enginePower);
  analogWrite(Motor_R_pwm_pin,enginePower);
  while(true){
  current = wiregetdegree();
  if(current <= target+2 && current >= target-2){
    digitalWrite(Motor_L_dir_pin, Motor_forward);
    digitalWrite(Motor_R_dir_pin, Motor_forward);
    analogWrite(Motor_L_pwm_pin,0);
    analogWrite(Motor_R_pwm_pin,0);
    target = -1;
    break;
  }
  pass = 1;
  return;
}
  //turn final
  if(target == -1) {int target = wiregetdegree()+90;}
  if(target > 360){
      target = target - 360;
    }
  digitalWrite(Motor_L_dir_pin, Motor_forward);
  digitalWrite(Motor_R_dir_pin, Motor_return);
  analogWrite(Motor_L_pwm_pin,enginePower);
  analogWrite(Motor_R_pwm_pin,enginePower);
  while(true){
  current = wiregetdegree();
  if(current <= target+2 && current >= target-2){
    digitalWrite(Motor_L_dir_pin, Motor_forward);
    digitalWrite(Motor_R_dir_pin, Motor_forward);
    analogWrite(Motor_L_pwm_pin,0);
    analogWrite(Motor_R_pwm_pin,0);
    target = -1;
    pass = -1;
    break;
  }
  }
  return;
}
}


struct match calcColDif(co *c){
  int colorDif;
  match colMatch{500,5};
  for(int i = 0; i < 4; i++){
    int r_diff = c->r - colors[i].r;
    int g_diff = c->g - colors[i].g;
    int b_diff = c->b - colors[i].b;
    colorDif = r_diff + g_diff + b_diff;
    if(colorDif < colMatch.dif){
      colMatch.dif = colorDif;
      colMatch.color = i;
    }
  }
  return colMatch;
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
    delay(200);
    Serial.println("Color recorded");
  }
  while(digitalRead(19) == HIGH){}
  //lidarAvg();
}

void loop() {
// while loop to wait until start button is pressed on website, and won't check again after starting.
  String message = Serial2.readStringUntil('\n');
  while(!start){
    String message = Serial2.readStringUntil('\n');
    if (message="start_program"){
      start = !start;
    }
}
  co currentColor;
  currentColor = RGBsensor();
  co *cptr = &currentColor; // Pointer to the current color
  match cuCol = calcColDif(cptr); //Return the preset it's the most similar to

if(cuCol.color == 0) {
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
  analogWrite(Motor_L_pwm_pin,50);
  analogWrite(Motor_R_pwm_pin,50);
  while(true){
  current = wiregetdegree();
  if(current <= target+2 && current >= target-2) {
  digitalWrite(Motor_L_dir_pin, Motor_forward);
  digitalWrite(Motor_R_dir_pin, Motor_forward);
  analogWrite(Motor_L_pwm_pin,0);
  analogWrite(Motor_R_pwm_pin,0);
  target=-1;
  break;
  }  
}
}
if(myLIDAR.readDistance() <= 12) {
  Obstacle();
}
if(cuCol.color == 1) {
  enginePower = 30;
}
else if(cuCol.color == 2) { //Detect green
  enginePower = 120;
  wallHunt = true;
}
else if(cuCol.color == 3) {
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
  analogWrite(Motor_L_pwm_pin,50);
  analogWrite(Motor_R_pwm_pin,50);
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
  co currentColor;
  currentColor = RGBsensor();
  co *cptr = &currentColor; // Pointer to the current color
  match cuCol = calcColDif(cptr);
  if (cuCol.color == 3){
    return;
  }
  /////////////////////////////////
  digitalWrite(Motor_L_dir_pin, Motor_forward);
  digitalWrite(Motor_R_dir_pin, Motor_return);
  analogWrite(Motor_L_pwm_pin,50);
  analogWrite(Motor_R_pwm_pin,50);
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
  co currentColor;
  currentColor = RGBsensor();
  co *cptr = &currentColor; // Pointer to the current color
  match cuCol = calcColDif(cptr);
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
  co currentColor;
  currentColor = RGBsensor();
  co *cptr = &currentColor; // Pointer to the current color
  match cuCol = calcColDif(cptr);
  if (cuCol.color == 2){
    return;
  }
  }
}
}
}
}