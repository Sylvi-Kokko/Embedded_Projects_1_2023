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

const int rs = 52, en = 53, d4 = 50, d5 = 51, d6 = 49, d7 = 48;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
int analogPin1 = A0;
int analogPin2 = A1;
int trimmer2 = A2;
int pwm_R, pwm_L, val, y_pwm, x_pwm, dir_L, dir_R, bigpulsecountright, bigpulsecountleft;
int LidMax = 5;
int st_Y = 503;
int st_X = 496;
int left_count = 0;
int right_count = 0;
int pulseDistR, pulseDistL;
const byte buttonPin = 19;
float x_akseli, val1, val2;
int i = 0, lidarDist, iterator = 0;
enum State {MOVE, SPIN, ZERO};
State movementState = ZERO;
int heading; // Sets the correct heading
int target = 40;
  
LIDARLite_v4LED myLIDAR;
int LidarVals[5];
DFRobot_TCS34725 tcs = DFRobot_TCS34725(&Wire, TCS34725_ADDRESS,TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
float LidarAvg(){ //Gathers lidar information to an array and produces an averaged Lidar value
 int j = LidMax;
  LidarVals[i]=myLIDAR.getDistance();
  i++;
  if(i==LidMax){
    i=0;
  }
  LidarVals[i]=myLIDAR.getDistance();
  i++;
  if(i==LidMax){
    i=0;
  }
 int tot = 0;
 if (LidarVals[4] == NULL){
  j=i;
 }
  for(int x=0; x<j; x++) {
    tot += LidarVals[x];
  }
  float LidAvg = tot/j;
  return LidAvg;
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
void count_reset() { //Count reset is called so that every new motion can start from 0
  left_count = 0;
  right_count = 0;
 }
void l_rising(){
  left_count += 1;
  bigpulsecountleft += 1;
 }
void r_rising(){
  right_count +=1;
  bigpulsecountright += 1;
 }
int left_turn(int cm){
  count_reset();
  digitalWrite(Motor_L_dir_pin, Motor_return);
  digitalWrite(Motor_R_dir_pin, Motor_forward); //Wheels rotate in opposite directions
  analogWrite(Motor_L_pwm_pin,150);
  analogWrite(Motor_R_pwm_pin,150);
  target = cm;
  movementState = MOVE;
  return 0;
 }

int right_turn(int cm){ //As with left but opposite
  count_reset();
  digitalWrite(Motor_R_dir_pin, Motor_return);
  digitalWrite(Motor_L_dir_pin, Motor_forward);
  analogWrite(Motor_L_pwm_pin,150);
  analogWrite(Motor_R_pwm_pin,150);
  target = cm;
  movementState = MOVE;
  return 0;
 }
int go_straight(int cm){
  count_reset();
  digitalWrite(Motor_L_dir_pin, Motor_forward); //Wheels move to the same direction
  digitalWrite(Motor_R_dir_pin, Motor_forward); //Otherwise same concept as left
  if(LidarAvg() < 30){
    analogWrite(Motor_L_pwm_pin,70);
    analogWrite(Motor_R_pwm_pin,70);
    }
  else{
    analogWrite(Motor_L_pwm_pin,255);
    analogWrite(Motor_R_pwm_pin,255);
    }
      target = cm;
  movementState = MOVE;
  return 0;
 }
int go_back(int cm){
  count_reset();
  digitalWrite(Motor_L_dir_pin, Motor_return);
  digitalWrite(Motor_R_dir_pin, Motor_return);
  if(LidarAvg() < 30){
    analogWrite(Motor_L_pwm_pin,70);
    analogWrite(Motor_R_pwm_pin,70);
    }
  else{
    analogWrite(Motor_L_pwm_pin,255);
    analogWrite(Motor_R_pwm_pin,255);
    }
      target = cm;
  movementState = MOVE;
  return 0;
 }
void turn_until(){
  int degree = wiregetdegree();
  if(heading>360) {
    heading = target-360;
  }
  if(heading < 0) {
    heading = target+360;  //Rollover to stay within 0-360
  }

  int angleDifference = heading - degree; //Calculate the shorter route to the asked for degree
  if (angleDifference < 180) {
    angleDifference -= 360;
  }

  if(angleDifference > 0){ //Set the direction of the motors according to the previous calculations
    digitalWrite(Motor_L_dir_pin, Motor_return);
  digitalWrite(Motor_R_dir_pin, Motor_forward);
  }else{
   digitalWrite(Motor_L_dir_pin, Motor_forward);
  digitalWrite(Motor_R_dir_pin, Motor_return);
  }
  analogWrite(Motor_L_pwm_pin,255);
  analogWrite(Motor_R_pwm_pin,255);
  if((degree+2 > heading) || (degree-2 < heading)){
    analogWrite(Motor_L_pwm_pin,0);
    analogWrite(Motor_R_pwm_pin,0);
    return;
  }
  else{
    turn_until();    
  }
}

int lidar_dist(int cm){ //Move so that lidar distance is +-2 from input
  int dist = LidarAvg();
  while((dist >= cm+2) || (dist <= cm-2)){
    if(dist > cm){
      digitalWrite(Motor_L_dir_pin, Motor_forward);
      digitalWrite(Motor_R_dir_pin, Motor_forward);
    }
    else{
      digitalWrite(Motor_L_dir_pin, Motor_return);
      digitalWrite(Motor_R_dir_pin, Motor_return);
    }
    if(LidarAvg() < 30){
    analogWrite(Motor_L_pwm_pin,70);
    analogWrite(Motor_R_pwm_pin,70);
    }
    else{
      analogWrite(Motor_L_pwm_pin,255);
      analogWrite(Motor_R_pwm_pin,255);
    }
    dist = LidarAvg();
  }
  analogWrite(Motor_L_pwm_pin,0);
  analogWrite(Motor_R_pwm_pin,0);
  count_reset();
  return 0;
 }
String RGBsensor(int opt){
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
  if(opt == 1){
    if(r > 100){
      return "red";
    }
    else if(b > 145){
      return "blue";
    }
    else{
      return "grey";
    }}
  else{
    String hexString = String((int)r, HEX) + String((int)g, HEX) + String((int)b, HEX);
    return hexString;
  }
  
 }


void pass(){
  count_reset();
  heading = wiregetdegree();
  digitalWrite(Motor_L_dir_pin, Motor_forward);
  digitalWrite(Motor_R_dir_pin, Motor_forward); //Wheels rotate in opposite directions
  analogWrite(Motor_L_pwm_pin,75);
  analogWrite(Motor_R_pwm_pin,150);
  delay(1000);
  turn_until();
  while(true){
    if(myLIDAR.getDistance() > 32){
    count_reset();
    analogWrite(Motor_L_pwm_pin,75);
    analogWrite(Motor_R_pwm_pin,75);
    digitalWrite(Motor_L_dir_pin, Motor_forward);
    digitalWrite(Motor_R_dir_pin, Motor_forward); //Wheels rotate in opposite directions
    analogWrite(Motor_L_pwm_pin,255);
    analogWrite(Motor_R_pwm_pin,255);
    delay(1000);
    analogWrite(Motor_L_pwm_pin,75);
    analogWrite(Motor_R_pwm_pin,75);
    return;
    }
  }  
}
String compdirection(int degree){ //Determine the letters to return with if statements that correspond to the correct directions
  if((degree>=0 && degree < 22.5)||(degree>=337.5)){
    return "N ";
  }else if(degree >= 22.5 && degree < 67.5){
    return "NE";
  }else if(degree >= 67.5 && degree < 112.5){
    return "E";
  }else if(degree >= 112.5 && degree < 157.5){
    return "SE";
  }else if(degree >= 157.5 && degree < 202.5){
    return "S";
  }else if(degree >= 202.5 && degree < 247.5){
    return "SW";
  }else if(degree >= 247.5 && degree < 292.5){
    return "W";
  }else if(degree >= 292.5 && degree < 337.5){
    return "NW";
  }else{
    return"null";
  }
 }

void setup() {
  Wire.begin();
  pinMode(buttonPin, INPUT);
  pinMode(Encoder_Int4, INPUT);
  pinMode(Encoder_Int5, INPUT);//Detects button being pressed
  attachInterrupt(digitalPinToInterrupt(2), r_rising, RISING); //Detects right wheel rotation
  attachInterrupt(digitalPinToInterrupt(3), l_rising, RISING); //Detects left wheel rotation
  Serial.begin(9600);
  Serial2.begin(9600);
  lcd.begin(20, 4);
  if (myLIDAR.begin() == false) {
    Serial.println("Device did not acknowledge! Freezing.");
    delay(1000);
  }
  Serial.println("LIDAR acknowledged!");
  while(!tcs.begin()){
    Serial.println("No TCS34725 found ... check your connections");
    delay(1000);
  }
}

void loop() {
  String message;
  String rgb = RGBsensor(0);
  lcd.setCursor(0,0);
  lcd.print(right_count);
  if(Serial2.available() > -1){
    iterator += 1;
    lcd.setCursor(0, 1);
    lcd.print(iterator);
    if(iterator % 3 == 0){
      Serial2.println("Lid="+String(LidarAvg()));
      delay(50);
      Serial2.println("Com="+String(wiregetdegree()));
      delay(50);
      Serial2.println("RGB="+rgb);
    }
    else{
      message = Serial2.readStringUntil('\n');
      Serial.println(message);
    }
    if(myLIDAR.getDistance() < 25){
      pass();
    }
    if(message.indexOf("Drive") > -1 ){
      if(message.indexOf("1") == 6){     
        movementState = MOVE;
        target = 500;
      }
      else if(message.indexOf("0") == 6){
        movementState=ZERO;
        target=0;
        analogWrite(Motor_L_pwm_pin, 0);
        analogWrite(Motor_R_pwm_pin,0);
      }
    }
    String col = RGBsensor(1);
    if(movementState == MOVE){
      if(col == "blue"){
      go_back(5);
      right_turn(34);
    }
    else if(col == "red"){
      go_back(5);
      left_turn(34);
    }
    else if(col == "grey"){
      digitalWrite(Motor_L_dir_pin, 1); //Here the values for direction and speed are actually sent to the motors
      digitalWrite(Motor_R_dir_pin, 1);
      if(lidarDist < 33){
        analogWrite(Motor_L_pwm_pin,75);
        analogWrite(Motor_R_pwm_pin,75);
      }
      else{
        analogWrite(Motor_L_pwm_pin,150);
        analogWrite(Motor_R_pwm_pin,150);            
      }
    }
    }
  }
  else{
    Serial.println("Esp not available");
    analogWrite(Motor_R_pwm_pin,0);
    analogWrite(Motor_L_pwm_pin,0);
  }
  switch(movementState){
    case MOVE:
      if(right_count > target*1.4){
        analogWrite(Motor_L_pwm_pin,0);
        analogWrite(Motor_R_pwm_pin,0);
        count_reset();
      }
      break;
    case SPIN:
      int degree = wiregetdegree();
      if(degree <= target + 1 && degree >= target - 1){
        analogWrite(Motor_L_pwm_pin,0);
        analogWrite(Motor_R_pwm_pin,0);
        count_reset();
      }
      break;
    }
  delay(100);
 }
