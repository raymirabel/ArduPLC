#include <ArduPLCm.h>
#include <avr/pgmspace.h>

// Carga la clase ArduPLCm
ArduPLCm plc;


void setup() 
{ 
  // Configura puerto serie...
  Serial.begin(9600); 
  
  // Mensaje inicial...
  Serial.println("---       Test placa ArduPLCm       ---");
  Serial.println("Ray Ingenieria Electronica, S.L. - 2012"); 
  Serial.println(); 
  Serial.println("Pulse 1,2,3 o 4 para activar los reles"); 
  Serial.println();   
  Serial.println();   
  delay(2000);
} 

void loop(){ 
  char c;
  int n;
  
  // Muestra la dirección del dipswotch
  Serial.print("DIPSWITCH: ");
  Serial.println(plc.leeDIPSW(),DEC);
  
  // Muestra los voltajes de las entradas analógicas
  Serial.print("AIN1: ");
  Serial.print(plc.leeVoltiosAIN(AIN1),2);  
  Serial.print("v - ");
  Serial.println(plc.leeAIN(AIN1),DEC);
  Serial.print("AIN2: ");
  Serial.print(plc.leeVoltiosAIN(AIN2),2);  
  Serial.print("v - ");
  Serial.println(plc.leeAIN(AIN2),DEC);
  Serial.print("AIN3: ");
  Serial.print(plc.leeVoltiosAIN(AIN3),2);  
  Serial.print("v - ");
  Serial.println(plc.leeAIN(AIN3),DEC);
  Serial.print("AIN4: ");
  Serial.print(plc.leeVoltiosAIN(AIN4),2);  
  Serial.print("v - ");
  Serial.println(plc.leeAIN(AIN4),DEC);

  // Muestra las entradas digitales  
  Serial.print("DIN1: ");
  Serial.println(plc.leeDIN(DIN1),DEC);
  Serial.print("DIN2: ");
  Serial.println(plc.leeDIN(DIN2),DEC);
  Serial.print("DIN3: ");
  Serial.println(plc.leeDIN(DIN3),DEC);
  Serial.print("DIN4: ");
  Serial.println(plc.leeDIN(DIN4),DEC);
  Serial.println();
 
  
  // Comprueba lo que recibes por puerto serie....
  for(n=0;n<=100;n++){
    if (Serial.available() > 0) {
        c = Serial.read();
        // Rele ON...
        if(c == '1'){
          plc.releOn(RELE1);
          delay(100);
          plc.releOff(RELE1);
        }
        if(c == '2'){
          plc.releOn(RELE2);
          delay(100);
          plc.releOff(RELE2);
        }
        if(c == '3'){
          plc.releOn(RELE3);
          delay(100);
          plc.releOff(RELE3);
        }
        if(c == '4'){
          plc.releOn(RELE4);
          delay(100);
          plc.releOff(RELE4);
        }
        Serial.flush();      
    } //if   
    delay(10);
  } //for
} 

