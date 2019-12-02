
/*
  ArduPLCm_Modbus.cpp
  06/02/2014 - v1.00
  - Versión inicial
  
  
  
  Librerias necesarias (copiar directamente en la carpeta librerías de Arduino):
  - ModbusSlave
  - ArduPLCm
  
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
  0x0005      int     R/W   00000      00015       00000     ---         Salidas reles en formato binario
  0x0006      int     R     00000      00063       00000     ---         Dirección modbus o estado del dipswitch  
*/
#include <ArduPLCm.h>
#include <ModbusSlave.h>
#include <avr/wdt.h> 

#define MAX_BUFFER_RX  15

// Mapa de registros modbus
enum {        
        MB_AN1,          // Entrada analogica 1
        MB_AN2,          // Entrada analogica 2
        MB_AN3,          // Entrada analogica 3
        MB_AN4,          // Entrada analogica 4
        MB_DIG_IN,       // Entradas digitales
        MB_RELAY,        // Salidas de rele
        MB_DIPSW,        // Estado del dipswitch
        MB_REGS	 	 // Numero total de registros
};
int regs[MB_REGS];	

// Carga la clase ArduPLCm
ArduPLCm plc;

// Crea la clase para el modbus...
ModbusSlave modbus;


void setup()  { 
  wdt_disable();
  
  // configura modbus...
  modbus.config(9600,'n');
  modbus.direccion = plc.leeDIPSW();

  // Activa WDT cada 4 segundos...   
//  wdt_enable(WDTO_4S); 
} 



void loop()  { 
  unsigned long curMillis = millis();          // Get current time

  // Asigna valores a la tabla modbus...
  regs[MB_AN1] = plc.leeAIN(AIN1);
  regs[MB_AN2] = plc.leeAIN(AIN2);
  regs[MB_AN3] = plc.leeAIN(AIN3);
  regs[MB_AN4] = plc.leeAIN(AIN4);  
  regs[MB_DIG_IN] = creaDINbyte(); 
  regs[MB_DIPSW] = plc.leeDIPSW();
      
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
  a0 = digitalRead(DIN1);  
  a1 = digitalRead(DIN2);  
  a2 = digitalRead(DIN3);  
  a3 = digitalRead(DIN4);  
  
  // Calcula byte...
  return(a0 + a1*2 + a2*4 + a3*8);
}

void actualizaReles(void){
  if((regs[MB_RELAY] & 0x01) == 0x01) plc.releOn(RELE1); else plc.releOff(RELE1);
  if((regs[MB_RELAY] & 0x02) == 0x02) plc.releOn(RELE2); else plc.releOff(RELE2);
  if((regs[MB_RELAY] & 0x04) == 0x04) plc.releOn(RELE3); else plc.releOff(RELE3);
  if((regs[MB_RELAY] & 0x08) == 0x08) plc.releOn(RELE4); else plc.releOff(RELE4);  
}
