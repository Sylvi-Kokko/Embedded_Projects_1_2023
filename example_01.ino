#include <LiquidCrystal.h>


const int rs = 52, en = 53, d4 = 50, d5 = 51, d6 = 49, d7 = 48;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
int x = 0;


void setup() {
  // put your setup code here, to run once:

  lcd.begin(20, 4);


}

void loop() {
  lcd.clear();

  x = analogRead(A1);

  int col = ((x / 822.0)*17)+1;
  lcd.setCursor(0, 0);
  lcd.print("X-position");
  lcd.setCursor(0, 1);
  lcd.print("-100%.....0%....100%");
  lcd.setCursor(col, 3);
  lcd.print("|");
  delay(10);


}

