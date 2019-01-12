/**************************************************************************
 @FILE:         Test_hmi.cpp
 @AUTHOR:       Raimundo Alfonso
 @COMPANY:      Ray Ingeniería Electronica, S.L.
 @DESCRIPTION:  Test ArduPLC NANO con HMI
  
 @BOARD SELECT: Arduino MICRO

 @VERSIONS:
  01-12-2016 - v1.00 :
  
**************************************************************************/

#include "U8glib.h"
#include "logo_ray.h"
#include <ArduPLCnano.h>

// Driver para OLED, azul = SH1106, blanco = SSD1306
//U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_DEV_0|U8G_I2C_OPT_NO_ACK|U8G_I2C_OPT_FAST); // Fast I2C / TWI 
U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_DEV_0|U8G_I2C_OPT_FAST); // Dev 0, Fast I2C / TWI

ArduPLCnano plc;

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
  Serial.println(F("A:xxx... Escribe texto de aviso en OLED")); 
}

String inString;
String cadenaAviso = "";

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

  // Configura puertos serie...
  Serial.begin(9600); 

  // Mensaje inicial...
  menu_inicio();

  // Menu de opciones...
  menu();
} 

void loop(){ 
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
      // Texto de aviso...
      if(inString.startsWith("A:")){
        cadenaAviso = inString.substring(2);
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
  int h, w, temp, n,lstr;
  char sTemp[30];
  static unsigned long timerMSG = millis();

  // Actualiza pantalla cada 100mS...
  if((millis() - timer_oled) >= 100){
    timer_oled = millis();
    u8g.firstPage();
    do{
      // Dibuja marcos...
      u8g.drawFrame(0,0,90,24);
      u8g.drawFrame(0,26,90,12);
      u8g.drawFrame(0,(h*4)-4,90,24);
      u8g.drawFrame(92,0,36,64);
  
      // Dibuja texto grande...
      u8g.setFont(u8g_font_9x15Br);  
      u8g.setFontRefHeightText();
      u8g.setFontPosTop();
      h = u8g.getFontAscent()-u8g.getFontDescent();
      u8g.drawStr(2, (h*1)-3 , "DIN..."); 
      u8g.drawStr(2, (h*4)-2 , "RELAY."); 
   
      // Visualiza entradas digitales...
      u8g.setFont(u8g_font_8x13r);  
      u8g.setFontRefHeightText();
      u8g.setFontPosTop();
      h = u8g.getFontAscent()-u8g.getFontDescent();
      w = 8;
      u8g.drawStr(1+(w*7), h*1 , "4321"); 
      for(n=0;n<=3;n++)
        u8g.drawFrame(1+(w*7)+(w*n),2,w-2,h-3);
      if(plc.digitalRead(DIN4))
        u8g.drawBox(1+(w*7)+(w*0),2,w-2,h-3);
      if(plc.digitalRead(DIN3))
        u8g.drawBox(1+(w*7)+(w*1),2,w-2,h-3);
      if(plc.digitalRead(DIN2))
        u8g.drawBox(1+(w*7)+(w*2),2,w-2,h-3);
      if(plc.digitalRead(DIN1))
        u8g.drawBox(1+(w*7)+(w*3),2,w-2,h-3);
  
      
      // Visualiza salidas rele...      
      u8g.drawStr(1+(w*7), (h*4)-2 , "1234");   
      for(n=0;n<=3;n++)
        u8g.drawFrame(1+(w*7)+(w*n),(h*5)-1,w-2,h-3);    
      if(plc.relayRead(RELAY1))
        u8g.drawBox(1+(w*7)+(w*0),(h*5)-1,w-2,h-3);
      if(plc.relayRead(RELAY2))
        u8g.drawBox(1+(w*7)+(w*1),(h*5)-1,w-2,h-3);
      if(plc.relayRead(RELAY3))
        u8g.drawBox(1+(w*7)+(w*2),(h*5)-1,w-2,h-3);
      if(plc.relayRead(RELAY4))
        u8g.drawBox(1+(w*7)+(w*3),(h*5)-1,w-2,h-3);
  
      // Visualiza botones...
      u8g.drawCircle(110,15,5);
      u8g.drawCircle(110,49,5);
      u8g.drawCircle(102,32,5);
      u8g.drawCircle(118,32,5);
      if(plc.digitalRead(BUTTON_UP))
        u8g.drawFilledEllipse(110,15,5,5);
      if(plc.digitalRead(BUTTON_DOWN))
        u8g.drawFilledEllipse(110,49,5,5);
      if(plc.digitalRead(BUTTON_LEFT))
        u8g.drawFilledEllipse(102,32,5,5);
      if(plc.digitalRead(BUTTON_RIGHT))
        u8g.drawFilledEllipse(118,32,5,5);
  
      // Escribe mensajes de aviso...
      u8g.setFont(u8g_font_6x10r);  
      u8g.setFontRefHeightText();
      u8g.setFontPosTop();    
      if((millis() - timerMSG) >= 500){   
        lstr = u8g.getStrWidth(cadenaAviso.c_str())/2;  
        u8g.drawStr(45 - lstr, 28 , cadenaAviso.c_str());   
      }
      if((millis() - timerMSG) >= 1000){
        timerMSG = millis();
      }
    }while(u8g.nextPage());
  }  
}

