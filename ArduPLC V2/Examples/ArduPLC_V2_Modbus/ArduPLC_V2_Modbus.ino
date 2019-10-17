
/*
  ArduPLC_V2_Modbus.cpp
  20/03/2019 - v1.00
  - Versión inicial
  
  
  
  Librerias necesarias (copiar directamente en la carpeta librerías de Arduino):
  - ModbusSlave
  - ArduPLC_V2
  
  Sketch para usar ArduPLCm como esclavo modbus RTU bajo RS485
  Copyright (c) 2014 Raimundo Alfonso
  Ray Ingeniería Electrónica, S.L.
  
  Este sketch está basado en software libre. Tu puedes redistribuir
  y/o modificarlo bajo los terminos de licencia GNU.

  Esta biblioteca se distribuye con la esperanza de que sea útil,
  pero SIN NINGUNA GARANTÍA, incluso sin la garantía implícita de
  COMERCIALIZACIÓN O PARA UN PROPÓSITO PARTICULAR.
  Consulte los terminos de licencia GNU para más detalles.
  
  * CARACTERISTICAS
  - Lectura de antradas analogicas
  - Lectura de entradas digitales
  - Escritura de reles
  - Parametros de comunicación RTU, 9600 baudios,n,8,1
  
  * MAPA MODBUS
    MODO R: FUNCION 3 - READ BLOCK HOLDING REGISTERS
    MODO W: FUNCION 6 - WRITE SINGLE HOLDING REGISTER
    
  DIRECCION   TIPO    MODO  FORMATO    MAXIMO      MINIMO    UNIDADES    DESCRIPCION
  ---------------------------------------------------------------------------------------------------------
  0x0000      int     R     00000      01023       00000     ---         Entrada analogica 1
  0x0001      int     R     00000      01023       00000     ---         Entrada analogica 2
  0x0002      int     R     00000      01023       00000     ---         Entrada analogica 3  
  0x0003      int     R     00000      01023       00000     ---         Entrada analogica 4
  0x0004      int     R     00000      00015       00000     ---         Entradas digitales en formato binario
  0x0007      int     R/W   00000      00015       00000     ---         Salidas reles en formato binario
  0x0006      int     R     00000      00063       00000     ---         Dirección modbus o estado del dipswitch  
*/
#include <ArduPLC_V2.h>
#include <ModbusSlave.h>
#include <avr/wdt.h> 

#define MAX_BUFFER_RX  15

// Mapa de registros modbus
enum {        
        MB_AN1,          // Entrada analogica 1
        MB_AN2,          // Entrada analogica 2
        MB_AN3,          // Entrada analogica 3
        MB_AN4,          // Entrada analogica 4
        MB_POT1,         // Estado del potenciometro 1
        MB_POT2,         // Estado del potenciometro 2        
        MB_DIG_IN,       // Entradas digitales
        MB_RELAY,        // Salidas de rele
        MB_REGS	 	 // Numero total de registros
};
int regs[MB_REGS];	

// Carga la clase ArduPLCm
ArduPLC_V2 plc;

// Crea la clase para el modbus...
ModbusSlave modbus;


void setup()  { 
  wdt_disable();
  
  // configura modbus...
  modbus.config(9600,'n');
  modbus.direccion = 1;

  // Activa WDT cada 4 segundos...   
//  wdt_enable(WDTO_4S); 
} 



void loop()  { 
  unsigned long curMillis = millis();          // Get current time

  // Asigna valores a la tabla modbus...
  regs[MB_AN1] = plc.leeAIN(1);
  regs[MB_AN2] = plc.leeAIN(2);
  regs[MB_AN3] = plc.leeAIN(3);
  regs[MB_AN4] = plc.leeAIN(4);  
  regs[MB_DIG_IN] = creaDINbyte(); 
  regs[MB_POT1] = plc.leePOT(1);
  regs[MB_POT2] = plc.leePOT(2);
      
  // Espera lectura de tramas modbus
  delay_modbus(100);
  
  // Actualiza los reles
  actualizaReles();
  
  // Reset WDT
  wdt_reset();
}

// Rutina de espera que atiende la tarea modbus...
void delay_modbus(int t){
  int n,tt;
  tt = t/10;
  
  for(n=0;n<=tt;n++){
    modbus.actualiza(regs,MB_REGS);
    delay(10);
  }  
}

byte creaDINbyte(void){
  byte a0,a1,a2,a3;

  // Lee dipswitch...
  a0 = digitalRead(1);  
  a1 = digitalRead(2);  
  a2 = digitalRead(3);  
  a3 = digitalRead(4);  
  
  // Calcula byte...
  return(a0 + a1*2 + a2*4 + a3*8);
}

void actualizaReles(void){
  if((regs[MB_RELAY] & 0x01) == 0x01) plc.releOn(1); else plc.releOff(1);
  if((regs[MB_RELAY] & 0x02) == 0x02) plc.releOn(2); else plc.releOff(2);
  if((regs[MB_RELAY] & 0x04) == 0x04) plc.releOn(3); else plc.releOff(3);
  if((regs[MB_RELAY] & 0x08) == 0x08) plc.releOn(4); else plc.releOff(4);  
}

