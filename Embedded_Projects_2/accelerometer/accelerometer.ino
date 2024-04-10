#include <DFRobot_LIS2DH12.h>

DFRobot_LIS2DH12 acce(&Wire,0x18);

void setup(void){

  Serial.begin(9600);
  while(!acce.begin()){
     Serial.println("Accelerometer initialization failed");
     delay(1000);
  }
  Serial.print("chip id : ");
  Serial.println(acce.getID(),HEX);
  acce.setRange(DFRobot_LIS2DH12::eLIS2DH12_16g);

  acce.setAcquireRate(DFRobot_LIS2DH12::eLowPower_10Hz);
  Serial.print("Acceleration:\n");
  delay(1000);
}
void loop(void){
  long ax,ay,az, ax0, ay0, az0;
  ax = acce.readAccX();//Get x direction acceleration
  ay = acce.readAccY();//Get y direction acceleration
  az = acce.readAccZ();//Get z direction acceleration
  Serial.print("Acceleration x: ");
  Serial.print(ax);
  Serial.print(" mg\t  y: ");
  Serial.print(ay);
  Serial.print(" mg\t  z: ");
  Serial.print(az);
  Serial.println(" mg");
  delay(300);
  if (ax0 == 0 || ax-ax0 > 3 || ax0-ax > 3) {
    ax0 = ax;
    Serial2.print("x=")
    Serial2.print(ax0)
  }
  if (ay0 == 0 || ay-ay0 > 3 || ay0-ay > 3) {
    ay0 = ay;
    Serial2.print("y=")
    Serial2.print(ay0)
  }
  if (az0 == 0 || az-az0 > 3 || az0-az > 3) {
    az0 = az;
    Serial2.print("z=")
    Serial2.print(az0)
  }
}