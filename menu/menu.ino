#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <TimerOne.h>
#include "buttonz.h"
#include "core.h"
#include "lan_if.h"
#include "rom_memory.h"
#include "telemetry_com.h"

unsigned char cur_mac[6] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDA, 0x02 };
unsigned char cur_ip[4] = { 192, 168, 0, 101 };
unsigned char cur_gateway[4] = { 192, 168, 5, 1 };
unsigned char cur_subnet[4] = { 255, 255, 255, 0 };
String idn_string = "Telemetry-command, 1, TESART, ver 1.0";

unsigned char rcvd_cmd_id;
unsigned char rcvd_parameter[6];

LiquidCrystal_I2C lcd(0x27, 16, 2);

Buttonz btn1(3, 100);
Buttonz btn2(7, 100);
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
  Serial.begin(9600);
  config_lan();

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

  ////////////Лево///////////////////
  if (btn1.state == BTN_PRESSED) { // если нажата 1 кнопка(левая кнопка)
    //задаем значение положения курсора
    if (corsor_pos == 1) {
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
    }
    btn2.state = BTN_CHECKED;
  }

  String text;
  char state;
  if (check_lan()) {
    Serial.println("LAN START");
    delay (1000);
    state = check_cmd(&rcvd_cmd_id, &rcvd_parameter[0]);
    switch (rcvd_cmd_id) {
      case REQ_IDN_ID:
        send_str_to_lan(&idn_string);
        Serial.println("1");
        break;
      case CMD_IN_SELECT0_ID:
        my_core.channel = 0;
        Serial.println("2");
        break;
      case CMD_IN_SELECT1_ID:
        my_core.channel = 1;
        Serial.println("3");
        break;
      case CMD_IN_SELECT2_ID:
        my_core.channel = 2;
        Serial.println("4");
        break;
      case REQ_IN_SELECT_ID:
        text = String(my_core.channel);
        send_str_to_lan(&text);
        Serial.println("5");
        break;
      case CMD_IN_POLARITY_POS_ID:
        my_core.polarity = 1;
        Serial.println("6");
        break;
      case CMD_IN_POLARITY_NEG_ID:
        my_core.polarity = 2;
        Serial.println("7");
        break;
      case REQ_SET_MAC_ADDR_ID:
        text = String(cur_mac[0], HEX) + ":" +  String(cur_mac[1], HEX) + ":" +  String(cur_mac[2], HEX) + ":" +  String(cur_mac[3], HEX) + ":" +  String(cur_mac[4], HEX) + ":" +  String(cur_mac[5], HEX);
        send_str_to_lan(&text);
        Serial.println("8");
        break;
      case REQ_SET_IP_ADDR_ID:
        text = String(cur_ip[0]) + "." + String(cur_ip[1]) + "." + String(cur_ip[2]) + "." + String(cur_ip[3]);
        send_str_to_lan(&text);
        Serial.println("9");
        break;
      case REQ_SET_IP_MASK_ID:
        text = String(cur_subnet[0]) + "." + String(cur_subnet[1]) + "." + String(cur_subnet[2]) + "." + String(cur_subnet[3]);
        send_str_to_lan(&text);
        Serial.println("10");
        break;
      case REQ_SET_DEF_GATEWAY_ID:
        text = String(cur_gateway[0]) + "." + String(cur_gateway[1]) + "." + String(cur_gateway[2]) + "." + String(cur_gateway[3]);
        send_str_to_lan(&text);
        Serial.println("11");
        break;
      default:
        Serial.println("DEF");
        break;


    }
    Serial.println("LAN END");
  }
  Serial.println("LOOP END");

}

void printCorsor(uint8_t str) {  // функция отрисовки курсора
  //получаем положение курсоса и отрисовываем
  switch (str) {
    case 1:
      lcd.setCursor(0, 1);
      lcd.print(" ");
      lcd.setCursor(0, 0);
      lcd.print(">");
      break;
    case 2:
      lcd.setCursor(0, 0);
      lcd.print(" ");
      lcd.setCursor(0, 1);
      lcd.print(">");
      break;
  }
}


void Timer_ISR() {
  btn1.CheckStatus();
  btn2.CheckStatus();
  btn3.CheckStatus();
}


void config_lan(void) {
  server_start(cur_mac, cur_ip, cur_gateway, cur_subnet);

  String text;
  text = String(cur_ip[0]) + "." + String(cur_ip[1]) + "." + String(cur_ip[2]) + "." + String(cur_ip[3]);
  lcd.setCursor(0, 0);
  lcd.print(text);
  text = String(cur_subnet[0]) + "." + String(cur_subnet[1]) + "." + String(cur_subnet[2]) + "." + String(cur_subnet[3]);
  lcd.setCursor(0, 1);
  lcd.print(text);
  delay(2000);
  lcd.clear();
}
