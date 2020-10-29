#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <TimerOne.h>
#include "buttonz.h"
#include "core.h"
#include "telemetry_com.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);

Buttonz btn1(3, 100);
Buttonz btn2(4, 100);
Buttonz btn3(6, 100);

core my_core;


uint8_t corsor_pos = 1;

void setup() {

  btn1.init();
  btn2.init();
  btn3.init();

  lcd.begin();
  lcd.backlight();
  lcd.setCursor(3, 0);
  lcd.print("TELEMETRY");
  lcd.setCursor(4, 1);
  lcd.print("COMMAND");
  delay(3000);
  lcd.clear();
  Timer1.initialize(100000);
  Timer1.attachInterrupt(Timer_ISR);
  interrupts();

}

void loop() {
  my_core.update();
  printCorsor(corsor_pos); // отрисовка курсора
  lcd.setCursor(1, 0);
  lcd.print("CHANNEL :");
  lcd.setCursor(10, 0);
  lcd.print(my_core.channel);

  lcd.setCursor(1, 1);
  lcd.print("POLARITY:");
  lcd.setCursor(10, 1);
  switch (my_core.polarity) {
    case 1:
      lcd.print("POS");
      break;
    case 2:
      lcd.print("NEG");
      break;

  }
  lcd.setCursor(14, 0);
  lcd.print("OK");

  ////////////Лево///////////////////
  if (btn1.state == BTN_PRESSED) { // если нажата 1 кнопка(левая кнопка)
    //задаем значение положения курсора
    if (corsor_pos == 1) {
      corsor_pos = 3;
    }
    else if (corsor_pos == 3) {
      corsor_pos = 2;
    }
    else if (corsor_pos == 2) {
      corsor_pos = 1;
    }
    btn1.state = BTN_CHECKED;
  }
 
  ///////////Право//////////////////
  if (btn3.state == BTN_PRESSED) { // если нажата 3 кнопка(левая кнопка)
    //задаем значение положения курсора
    if (corsor_pos == 1) {
      corsor_pos = 2;
    }
    else if (corsor_pos == 2) {
      corsor_pos = 3;
    }
    else if (corsor_pos == 3) {
      corsor_pos = 1;
    }
    btn3.state = BTN_CHECKED;
  }

  /////////////Центр//////////////////////
  if (btn2.state == BTN_PRESSED) { // если нажата 2 кнопка (центральная)
    // исходя из положения курсора происходят действия
    switch (corsor_pos) {
      case 1:
        ++my_core.channel; // задаем канал
        break;
      case 2:
        ++my_core.polarity; // задаем полярность
        break;
      case 3: // когда курсор в положении "ОК"
        //"ОК"
        lcd.clear();
        lcd.setCursor(5,0);
        lcd.print("START");
        delay(2000);
        break;

    }
    btn2.state = BTN_CHECKED;
  }



}

void printCorsor(uint8_t str) {  // функция отрисовки курсора
  //получаем положение курсоса и отрисовываем
  switch (str) {
    case 1:
      lcd.setCursor(0, 1);
      lcd.print(" ");
      lcd.setCursor(13, 0);
      lcd.print(" ");
      lcd.setCursor(0, 0);
      lcd.print(">");
      break;
    case 2:
      lcd.setCursor(0, 0);
      lcd.print(" ");
      lcd.setCursor(13, 0);
      lcd.print(" ");
      lcd.setCursor(0, 1);
      lcd.print(">");
      break;
    case 3:
      lcd.setCursor(0, 0);
      lcd.print(" ");
      lcd.setCursor(0, 1);
      lcd.print(" ");
      lcd.setCursor(13, 0);
      lcd.print(">");
      break;
  }
}


void Timer_ISR() {
  btn1.CheckStatus();
  btn2.CheckStatus();
  btn3.CheckStatus();


}
