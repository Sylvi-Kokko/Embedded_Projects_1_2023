#include <LiquidCrystal.h>
const int rs = 52, en = 53, d4 = 50, d5 = 51, d6 = 49, d7 = 48;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
int val = 0;
#define Motor_forward         0
#define Motor_return          1
#define Motor_L_dir_pin       7
#define Motor_R_dir_pin       8
#define Motor_L_pwm_pin       9
#define Motor_R_pwm_pin       10
void setup(){
  Serial.begin(9600);
  lcd.begin(20, 4);
  Serial.println("Write something to the serial monitor.");
}
void loop(){
  if (Serial.available() > 0){
    String message = Serial.readStringUntil('\n');
    Serial.print("Message received, content: ");
    Serial.println(message);
    int pos_s;
    int dist = message.indexOf("dist");
    int lcdprint = message.indexOf("LCD");
    int serialprint = message.indexOf("Print");
    if (dist > -1){
      Serial.println("Command = distance ");
      pos_s = message.indexOf(":");
      if (pos_s > -1){
        String stat = message.substring(pos_s + 1);
        val = stat.toInt();
          lcd.setCursor(0, 0);
          lcd.clear();
          lcd.print(val);
      }
    }
    else if (lcdprint > -1){
      Serial.println("Command = LCD ");
      pos_s = message.indexOf(":");
      if (pos_s > -1){
        String stat = message.substring(pos_s + 1);
          lcd.setCursor(0, 0);
          lcd.clear();
          lcd.print(stat);
      }
    }else if (serialprint > -1){
      Serial.println("Command = Print ");
      pos_s = message.indexOf(":");
      if (pos_s > -1){
        String stat = message.substring(pos_s + 1);
        if (stat == "Hi" || stat == "hi") {
          Serial.println("Hi!");
        }
        else if (stat == "Hello") {
          Serial.println("Hello there!");
        }
      }
    }
    else{
      Serial.println("No greeting found, try typing Print:Hi or Print:Hello\n");
    }
    if(val > 0){
    digitalWrite(Motor_L_dir_pin, 1);
    analogWrite(Motor_L_pwm_pin, 700);
    digitalWrite(Motor_R_dir_pin, 1);
    analogWrite(Motor_R_pwm_pin, 700);
    delay(104*val);
    }else if (val < 0){
    digitalWrite(Motor_L_dir_pin, 0);
    analogWrite(Motor_L_pwm_pin, 700);
    digitalWrite(Motor_R_dir_pin, 0);
    analogWrite(Motor_R_pwm_pin, 700);
    delay(104*(-val));
    }
    analogWrite(Motor_R_pwm_pin, 0);
    analogWrite(Motor_L_pwm_pin, 0);
  }
}