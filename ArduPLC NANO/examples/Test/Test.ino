/**************************************************************************
 @FILE:         Test.cpp
 @AUTHOR:       Raimundo Alfonso
 @COMPANY:      Ray Ingeniería Electronica, S.L.
 @DESCRIPTION:  Test ArduPLC NANO sin HMI
  
 @BOARD_SELECT: Arduino MICRO

 @VERSIONS:
  01-12-2016 - v1.00 :
  
**************************************************************************/
#include <ArduPLCnano.h>

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
}

String inString;

void setup(){ 
  delay(4000); 

  // Configura puertos serie...
  Serial.begin(9600); 

  // Mensaje inicial...
  menu_inicio();

  // Menu de opciones...
  menu();
} 

void loop(){ 
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
}

