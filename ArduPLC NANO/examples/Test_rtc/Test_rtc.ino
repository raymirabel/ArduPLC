/**************************************************************************
 @FILE:         Test_rtc.cpp
 @AUTHOR:       Raimundo Alfonso
 @COMPANY:      Ray Ingeniería Electronica, S.L.
 @DESCRIPTION:  Test ArduPLC NANO con HMI y rtc basado en el chip DS3231
                Necesitarás la librería RTClib.h de adafruit ( https://github.com/adafruit/RTClib )
  
 @BOARD SELECT: Arduino MICRO

 @VERSIONS:
  01-12-2016 - v1.00 :
  
**************************************************************************/
#include "RTClib.h"
#include "U8glib.h"
#include "logo_ray.h"
#include <ArduPLCnano.h>

// Driver para OLED, azul = SH1106, blanco = SSD1306
//U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_DEV_0|U8G_I2C_OPT_NO_ACK|U8G_I2C_OPT_FAST); // Fast I2C / TWI 
U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_DEV_0|U8G_I2C_OPT_FAST); // Dev 0, Fast I2C / TWI

ArduPLCnano plc;

RTC_DS3231 rtc;
char daysOfTheWeek[7][18] = {"D O M I N G O", "L U N E S", "M A R T E S", "M I E R C O L E S", "J U E V E S", "V I E R N E S", "S A B A D O"};
DateTime now;

void menu_inicio(void){
  Serial.println(F("---    Test placa ArduPLC NANO     ---"));
  Serial.println(F("   Ray Ingenieria Electronica, S.L."));  
  Serial.println();
  Serial.println(F("Pulse '?' para menu de ayuda (activa NL & CR)."));    
}

void menu(void){
  Serial.println(F("                MENU PRINCIPAL"));
  Serial.println(F("-----------------------------------------------"));
  Serial.println(F("Rn       Activa rele n (n de 1 a 4)"));
  Serial.println(F("Fn       Desactiva rele n (n de 1 a 4)"));
  Serial.println(F("D        Lee entradas digitales"));
  Serial.println(F("L        Lee fecha y hora")); 
}

String inString;

void setup(){ 
  // Configura OLED...
  u8g.begin();
  u8g.setFont(u8g_font_9x15Br); 
  u8g.setFontRefHeightText();
  u8g.setFontPosTop();

  // Visualiza logo en pantalla OLED...
  u8g.firstPage();  
  do {
    u8g.drawBitmapP( 0, 0, 16, 20, logo_ray);   
    u8g.drawBox(0, 28, 128, 16); 
    u8g.setDefaultBackgroundColor(); 
    u8g.drawStr(10,30, "ArduPLC NANO");   
    u8g.setDefaultForegroundColor();      
    u8g.drawStr(19,50, "Ray - 2016");     
  } while( u8g.nextPage() );  
  delay(4000); 

  // Configura RTC...
  if (!rtc.begin()) {
    Serial.println("No puedo encontrar el módulo RTC.");
    while (1);
  }
  if (rtc.lostPower()) {
    Serial.println("El módulo RTC perdió la alimentación, por lo que lo ponemos en hora.");
    // Pon en hora el módulo RTC a la hora de compilar...
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  // Configura puertos serie...
  Serial.begin(9600); 

  // Mensaje inicial...
  menu_inicio();

  // Menu de opciones...
  menu();
} 

void loop(){ 
  // Lee RTC...
  now = rtc.now();
  
  // HMI...
  gestiona_oled();

  // Comprueba lo que recibes por puerto serie....
  while (Serial.available() > 0) {
    int c = Serial.read();
    inString += (char)c; 
    if (c == 0x0D) {
      // Quita el retorno de carro...
      inString.trim();
      // comprueba el comando...
      // Menu...
      if(inString == "?"){
        menu();
      }
      // Lee RTC...
      if(inString.startsWith("L")){
        leeRTC();
        Serial.println("OK");
      }
      // Rele ON...
      if(inString.startsWith("R")){
        releOn(inString.charAt(1));
        Serial.println("OK");
      }
      // Rele OFF...
      if(inString.startsWith("F")){
        releOff(inString.charAt(1));
        Serial.println("OK");        
      }   
      // Lee entradas digitales...
      if(inString.startsWith("D")){
        digitalR();
        Serial.println("OK");        
      }    
      inString = ""; 
      Serial.flush();
    }    
  }
} 


void leeRTC(void){
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") - ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
}

void releOn(char rele){
  byte r;
  r = byte(rele) - 0x30;
  if(r == 1) plc.relayWrite(RELAY1, ON);
  if(r == 2) plc.relayWrite(RELAY2, ON); 
  if(r == 3) plc.relayWrite(RELAY3, ON);  
  if(r == 4) plc.relayWrite(RELAY4, ON);
}

void releOff(char rele){
  byte r;
  r = byte(rele) - 0x30;
  if(r == 1) plc.relayWrite(RELAY1, OFF);
  if(r == 2) plc.relayWrite(RELAY2, OFF);
  if(r == 3) plc.relayWrite(RELAY3, OFF);
  if(r == 4) plc.relayWrite(RELAY4, OFF);
}

void digitalR(){
  Serial.print("DIN1 : ");
  Serial.println(plc.digitalRead(DIN1));

  Serial.print("DIN2 : ");
  Serial.println(plc.digitalRead(DIN2));

  Serial.print("DIN3 : ");
  Serial.println(plc.digitalRead(DIN3));
  
  Serial.print("DIN4 : ");
  Serial.println(plc.digitalRead(DIN4));

  Serial.print("BUTTON_UP    : ");
  Serial.println(plc.digitalRead(BUTTON_UP));

  Serial.print("BUTTON_DOWN  : ");
  Serial.println(plc.digitalRead(BUTTON_DOWN));

  Serial.print("BUTTON_RIGHT : ");
  Serial.println(plc.digitalRead(BUTTON_RIGHT));

  Serial.print("BUTTON_LEFT  : ");
  Serial.println(plc.digitalRead(BUTTON_LEFT));

}

void gestiona_oled(void){
  static unsigned long timer_oled = 0;
  int h, w, temp, n;
  char sTemp[30];
  static unsigned long timerMSG = millis();
  int x,y;

  // Actualiza pantalla cada 100mS...
  if((millis() - timer_oled) >= 100){
    timer_oled = millis();
    u8g.firstPage();
    do{
      // Escribe fecha y hora...
      u8g.setFont(u8g_font_6x10r);  
      u8g.setFontRefHeightText();
      u8g.setFontPosTop();    
      sprintf(sTemp,"%02d/%02d/%04d - %02d:%02d:%02d",now.day(), now.month(), now.year(), now.hour(),now.minute(), now.second());
      u8g.drawStr(0, 0 , sTemp);   
      sprintf(sTemp,"%s",daysOfTheWeek[now.dayOfTheWeek()]);
      x = 64 - ((strlen(sTemp) * 6) / 2);
      u8g.drawStr(x, 55 , sTemp);        

      // Dibuja reloj analógico...
      // esfera...
      u8g.drawCircle(64,32,21,U8G_DRAW_ALL);
      u8g.drawCircle(64,32,20,U8G_DRAW_ALL);
      for(n=0;n<12;n++){
        x = 19*sin((2*PI*n)/12);
        y = 19*cos((2*PI*n)/12);       
        u8g.drawPixel(64+x,32-y);
      }
      // hora...
      x = 12*sin((2*PI*now.hour())/12);
      y = 12*cos((2*PI*now.hour())/12);
      u8g.drawLine(64,32,64+x,32-y);
      // minutos...
      x = 18*sin((2*PI*now.minute())/60);
      y = 18*cos((2*PI*now.minute())/60);
      u8g.drawLine(64,32,64+x,32-y);
      // segundos...
      x = 18*sin((2*PI*now.second())/60);
      y = 18*cos((2*PI*now.second())/60);
      u8g.drawLine(64,32,64+x,32-y);
    }while(u8g.nextPage());
  }  
}

