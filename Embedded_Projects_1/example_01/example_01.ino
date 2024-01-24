#define Motor_forward         0
#define Motor_return          1
#define Motor_L_dir_pin       7
#define Motor_R_dir_pin       8
#define Motor_L_pwm_pin       9
#define Motor_R_pwm_pin       10

void setup()
{
    Serial.begin(9600);
}

void loop()                    
{           
  Serial.println(analogRead(A0));
  Serial.println("A0: ^");
  Serial.println(analogRead(A1));
  Serial.println("A1: ^");
  delay(1000);
    /* int pwm_R=0;
    int pwm_L=0;  
    for(int i = 99;i<100;i--){
      
////// Direction
       digitalWrite(Motor_R_dir_pin,Motor_return);  
       digitalWrite(Motor_L_dir_pin,Motor_return);
       delay(2000);
       digitalWrite(Motor_R_dir_pin,Motor_forward);  
       digitalWrite(Motor_L_dir_pin,Motor_forward); 
       delay(2000);
////// Pwm
      int pwm_R=0;
      int pwm_L=0; 
      Serial.println(pwm_L);
      Serial.println(pwm_R);
      pwm_L = i;
      pwm_R = i;
      Serial.println(pwm_L);
      Serial.print(pwm_R);
      analogWrite(Motor_L_pwm_pin,pwm_L);
      analogWrite(Motor_R_pwm_pin,pwm_R);    */         
   /* } */                     
}                                   
