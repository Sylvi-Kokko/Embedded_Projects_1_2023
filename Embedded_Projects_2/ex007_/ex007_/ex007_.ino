/**
* @file Ex001_Week0.ino
* @authors Dordze Ostrowski, Sylvi Kokko, Wilhelm Nilsson
* @brief Project for Embedded Projects course (TAMK, Spring 2024)
* @date 2024-24-01
*/

#include <LiquidCrystal.h>
#include <Wire.h>
#include "LIDARLite_v4LED.h"
#include <EEPROM.h>
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
int st_Y = 503;
int st_X = 496;
int left_count = 0;
int right_count = 0;
int pulseDistR, pulseDistL;
const byte buttonPin = 19;
bool steering_mode = true;
bool isTrimmer =  false;
bool correct = false; // Whether to correct heading
int heading; // Sets the correct heading
int follow_dist = -1;
float x_akseli, val1, val2;
LIDARLite_v4LED myLIDAR;
float newDistance;
float encoderCalibrationLeft = 14;
float encoderCalibrationRight = 14;
enum State {MOVE, SPIN, ZERO};
State movementState = ZERO;
int target, i=0;
int address = 0;
int LidarVals[20];
int calibration_l[20], calibration_r[20];

void buttonPressed(){ 
  if (steering_mode){
    steering_mode = false;
  }
  else if (!steering_mode){
    steering_mode = true;
  }
  analogWrite(Motor_L_pwm_pin,0);
  analogWrite(Motor_R_pwm_pin,0);
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

//Compass reading
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

//Movement functions 
int left_turn(int cm){
  count_reset();
  digitalWrite(Motor_L_dir_pin, Motor_return); 
  digitalWrite(Motor_R_dir_pin, Motor_forward); //Wheels rotate in opposite directions
  analogWrite(Motor_L_pwm_pin,255);
  analogWrite(Motor_R_pwm_pin,255);
  target = cm;
  movementState = MOVE;
  return 0;
}

int right_turn(int cm){ //As with left but opposite
  count_reset();
  digitalWrite(Motor_R_dir_pin, Motor_return);
  digitalWrite(Motor_L_dir_pin, Motor_forward);
  analogWrite(Motor_R_pwm_pin,255);
  analogWrite(Motor_L_pwm_pin,255);
  target = cm;
  movementState = MOVE;
  return 0;
}
int go_straight(int cm){
  count_reset();  
  digitalWrite(Motor_L_dir_pin, Motor_forward); //Wheels move to the same direction
  digitalWrite(Motor_R_dir_pin, Motor_forward); //Otherwise same concept as left
  analogWrite(Motor_L_pwm_pin,255);
  analogWrite(Motor_R_pwm_pin,255);
  target = cm;
  movementState = MOVE;
  return 0;
}
int go_back(int cm){ 
  count_reset();
  digitalWrite(Motor_L_dir_pin, Motor_return);
  digitalWrite(Motor_R_dir_pin, Motor_return);
  analogWrite(Motor_L_pwm_pin,255);
  analogWrite(Motor_R_pwm_pin,255);
  target = cm;
  movementState = MOVE;
  return 0;
}
int turn_until(float targe){ //
  int degree = wiregetdegree();


  if(targe>360) {
    targe = target-360;    
  }
  if(targe < 0) {
    targe = target+360;  //Rollover to stay within 0-360
  }

  int angleDifference = targe - degree; //Calculate the shorter route to the asked for degree
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
  target = targe;
  movementState = SPIN;  
  return 0;
}
int lidar_dist(int cm){
  int dist = myLIDAR.getDistance();
  heading = wiregetdegree();
  while((dist >= cm+2) || (dist <= cm-2)){
    if(dist > cm){
      digitalWrite(Motor_L_dir_pin, Motor_forward);
      digitalWrite(Motor_R_dir_pin, Motor_forward);
    }
    else{
      digitalWrite(Motor_L_dir_pin, Motor_return);
      digitalWrite(Motor_R_dir_pin, Motor_return);
    }
    analogWrite(Motor_L_pwm_pin,255);
    analogWrite(Motor_R_pwm_pin,255);
    dist = myLIDAR.getDistance();
  }
  analogWrite(Motor_L_pwm_pin,0);
  analogWrite(Motor_R_pwm_pin,0);
  count_reset();
  return 0;
}

float LidarAvg(){ //Gathers lidar information to an array and produces an averaged Lidar value
LidarVals[i]=myLIDAR.getDistance();
i++;
if(i==20){
  i=0;
}
int tot = 0;
for(int x=0; x<20; x++) {
  tot += LidarVals[x];
}
float LidAvg = tot/20;
return LidAvg;
}

void measurement(int height){ //Measures lidar distance
  int init = wiregetdegree();
  int xpos = LidarAvg();
  Serial.print("xpos=");
  Serial.print(xpos);
  turn_until(init+90);
  int ypos = LidarAvg();
  Serial.print("ypos=");
  Serial.print(ypos);
  turn_until(init+180);
  int xneg = LidarAvg();
  Serial.print("xneg=");
  Serial.print(xneg);
  turn_until(init+270);
  int yneg = LidarAvg();
  Serial.print("yneg=");
  Serial.print(yneg);
  float area = (xpos+xneg)*(ypos+yneg);
  float volume = area*height;
  lcd.clear();
  Serial.print("xpos=");
  Serial.print(xpos);
  Serial.print("ypos=");
  Serial.print(ypos);
  Serial.print("xneg=");
  Serial.print(xneg);
  Serial.print("yneg=");
  Serial.print(yneg);
  lcd.setCursor(0,1);
  lcd.print("Area= ");
  lcd.print(area);
  lcd.print("cm^2");
  lcd.setCursor(0, 2);
  lcd.print("Volume= ");
  lcd.print(volume);
  lcd.print("cm^3");
  delay(6000);
  lcd.clear();
}
void wifisteering(){ //Controlling the motion through wifi
 val = 0;
    lcd.setCursor(0, 0);
     if (Serial2.available() > 0){
    String message = Serial.readStringUntil('\n');//Read one line from serial
    Serial2.print("Message received, content: ");
    Serial2.println(message); 
    int pos_s;
    int poz_z;
    int movement = message.indexOf("Move");
    int turn = message.indexOf("Turn");
    int until = message.indexOf("UNTIL");
    int followDist = message.indexOf("Follow");
    int followTrim = message.indexOf("Trimmer");
    int ex4 = message.indexOf("ex4");
    int cali = message.indexOf("Calibrate");
    int measure = message.indexOf("Measure");
    int correction = message.indexOf("Correct");
    if (movement > -1){ //If the command was movement, index will be bigger than -1
      Serial2.println("Command = movement ");
      pos_s = message.indexOf(":");
      if (pos_s > -1){ //If the index is bigger than -1 it exists
        String stat = message.substring(pos_s + 1); // Get the string from after the ':'
        val = stat.toInt(); //Get the int from it
        if(val<0){ //Call the movement function with the correct distance
          go_back((-1)*val);
        }else{
          go_straight(val);
        }
      }
    }else if (turn > -1){ //If turn was called 
      Serial2.println("Command = TURN ");
      pos_s = message.indexOf(":");
      if (pos_s > -1){ //Same as above but right or left turn is called
        String stat = message.substring(pos_s + 1);
        val = stat.toInt();
        if(val < 0){
          right_turn((-1)*val);
        }
        else{left_turn(val);}
      }
    }else if (until > -1){ //
      Serial2.println("Command = UNTIL ");
      pos_s = message.indexOf(":");
      if (pos_s > -1){ //Same as above but turn until is called 
        String stat = message.substring(pos_s + 1);
        val = stat.toInt();
        turn_until(val); 
      }
    }else if (followDist > -1){
      Serial2.println("Command = Follow ");
      pos_s = message.indexOf(":");
      if (pos_s > -1){
        String stat = message.substring(pos_s + 1);
        val = stat.toInt();
        follow_dist = val;  
        isTrimmer = false;      
      }      
    }else if (correction > -1){
      String stat = message.substring(pos_s +1 );
      correct != correct;
      heading = wiregetdegree();
      Serial2.println("Command = Correct " + correct);
    }else if (followTrim > -1){
      Serial2.println("Command = Trimmer ");
      String stat = message.substring(pos_s + 1);
      val = analogRead(trimmer2);
      follow_dist = val/50;
      isTrimmer = true;
    }else if (measure > -1){
      Serial2.println("Command = Measurement ");
      pos_s = message.indexOf(":");
      if (pos_s > -1){
        String stat = message.substring(pos_s + 1);
        val = stat.toInt();
      measurement(val);
      }
    }else if (cali > -1){
      Serial2.println("Command = Calibrating ");
      pos_s = message.indexOf(":");
      calibrate();
    }else if (ex4 > -1){
      Serial2.println("Command = Exercise 4 ");
      pos_s = message.indexOf(":");
      exe2();
    }else{
      Serial2.println("No greeting found, try typing Print:Hi or Print:Hello\n");
    }
  }
}
void joysticksteering(){ //Read the values from the joystick and move the wheels
    val1 = analogRead(analogPin2);
    val2 = analogRead(analogPin1);
    val2 = val2 + 15.5; //The joystick is a bit off so these correct it to 0 
    val1 = val1 + 7.5;
    x_pwm = map(val2, 0, 1023, -90, 90);
    y_pwm = map(val1, 0, 1023, -160, 160);
    if((val1 < 509.5)){ //Go backwards 
      dir_L = 1; //Set the direction
      dir_R = 1;
      if(x_pwm < -1){
        pwm_L = -1*x_pwm + abs(y_pwm); //Set the power by combining both axes 
        pwm_R = x_pwm + abs(y_pwm);
      }else{
        pwm_L = -1*x_pwm + abs(y_pwm);
        pwm_R = x_pwm + abs(y_pwm);
      }
      
    }
    else if ((val1 > 513.5)){ //Same as above but the opposite direction
      dir_L = 0;
      dir_R = 0; 
      if(x_pwm < -1){
        pwm_L = -1*x_pwm + abs(y_pwm);
        pwm_R = x_pwm + abs(y_pwm);
      }else{
        pwm_L = -1*x_pwm + abs(y_pwm);
        pwm_R = x_pwm + abs(y_pwm);
      }
    }else{ //If the y is close enough to middle, just turn left or right
      if(x_pwm > 2){ 
      dir_L = 0; //Opposite directions make for more efficient turning
      dir_R = 1;
      pwm_L = -x_pwm; //When just turning, the power from x axis is enough
      pwm_R = x_pwm; 
      }else if(x_pwm < -2){
      dir_L = 1;
      dir_R = 0;
      pwm_L = x_pwm;
      pwm_R = -x_pwm;
      }else{
      pwm_L = 0; 
      pwm_R = 0;
      }
      
    }
    digitalWrite(Motor_L_dir_pin, dir_L); //Here the values for direction and speed are actually sent to the motors
    digitalWrite(Motor_R_dir_pin, dir_R);
    
    analogWrite(Motor_L_pwm_pin,pwm_L);
    analogWrite(Motor_R_pwm_pin,pwm_R);
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

void exe2(){
  count_reset();  
  lidar_dist(20);
  turn_until(90);
  lidar_dist(25);
  left_turn(90);
  lidar_dist(20);
  left_turn(90);
  lidar_dist(25);
  left_turn(90);
  lidar_dist(20);
}

void calibrate(){
  encoderCalibrationLeft = 14;
  EEPROM.update(address, encoderCalibrationLeft);
  address = address + 1;
  if (address == EEPROM.length()) {
    address = 0;
  }
  encoderCalibrationRight = 14;
  EEPROM.update(address, encoderCalibrationRight);
  address = address + 1;
  if (address == EEPROM.length()) {
    address = 0;
  }
  byte value = EEPROM.read(address);

  Serial.print(address);
  Serial.print("\t");
  Serial.print(value, DEC);
  Serial.println();  
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
    while(1);
  }
  Serial.println("LIDAR acknowledged!");
}

void loop() {
  if (steering_mode){ //Wifi / Serial steering. Controlled with the button.
    wifisteering();
    lcd.setCursor(0, 0);
    lcd.print("Steerviawifi/serial");
    lcd.setCursor(0, 1);
    lcd.print("Dist(cm):");
    lcd.print(bigpulsecountleft/encoderCalibrationLeft);
    lcd.print(" ");
    lcd.print(bigpulsecountright/encoderCalibrationRight);
    lcd.setCursor(0, 2);
    lcd.print("CountsL/R:" );
    lcd.print(bigpulsecountleft);
    lcd.print(" ");
    lcd.print(bigpulsecountright);
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
  }
  else{ //Joystick steering
    joysticksteering();
    lcd.setCursor(0, 0);
    lcd.print("Steerviajoystick");
    lcd.setCursor(0, 1);
    lcd.print("DistL/R(cm):");
    lcd.print(bigpulsecountleft/encoderCalibrationLeft);
    lcd.print(" ");
    lcd.print(bigpulsecountright/encoderCalibrationRight);
    lcd.setCursor(0, 2);
    lcd.print("CountsL/R:" );
    lcd.print(bigpulsecountleft);
    lcd.print(" ");
    lcd.print(bigpulsecountright);
  }
  newDistance = LidarAvg()-5;
  if (follow_dist > 0) {
    if (!isTrimmer){
      if (newDistance+1 < follow_dist) {go_straight(1);}
      else if (newDistance-1 > follow_dist) {go_back(1);}
    }
    else {
      follow_dist = analogRead(A2)/50;
      if (newDistance+1 < follow_dist) {go_straight(1);}
      else if (newDistance-1 > follow_dist) {go_back(1);}
    }
  }
  if (correct){
    if(wiregetdegree() != heading) {
      turn_until(heading);
    }
  }
  lcd.setCursor(0, 3);
  lcd.print("Compass = ");
  lcd.print(wiregetdegree());
  lcd.print(" ");
  lcd.print(compdirection(wiregetdegree()));
  delay(100);
  lcd.clear();
}
