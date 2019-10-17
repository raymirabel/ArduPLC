// Programa de test para la placa ArduPLC - v2.00
// Raimundo Alfonso Sanchez
// Ray Ingeniería Electrónica, S.L. - 2013

#include <SD.h>
#include <Wire.h>
#include <Rtc_Pcf8563.h>
#include <avr/pgmspace.h>

Rtc_Pcf8563 rtc;
const int chipSelect = 2;
char filename[] = "LOGGER00.CSV";
File logfile;
boolean sdReady = false;

String inString = "";

prog_uchar pr1[] PROGMEM   = {"                MENU PRINCIPAL"};
prog_uchar pr2[] PROGMEM   = {"-----------------------------------------------"};
prog_uchar pr3[] PROGMEM   = {"RT        Lee fecha y hora"};
prog_uchar pr4[] PROGMEM   = {"WDddmmaa  Escribe fecha en el RTC"};
prog_uchar pr5[] PROGMEM   = {"WThhmmss  Escribe hora en el RTC"};
prog_uchar pr6[] PROGMEM   = {"RC        Lee temperatura"};
prog_uchar pr7[] PROGMEM   = {"ROn       Activa rele n (n de 1 a 4)"};
prog_uchar pr8[] PROGMEM   = {"RFn       Desactiva rele n (n de 1 a 4)"};
prog_uchar pr9[] PROGMEM   = {"RD        Lee entradas digitales"};
prog_uchar pr10[] PROGMEM  = {"RA        Lee entradas analogicas"};

prog_uchar pr20[] PROGMEM  = {"---    Test placa ArduPLC v2.00    ---"};
prog_uchar pr21[] PROGMEM  = {"   Ray Ingenieria Electronica,S.L."};
prog_uchar pr22[] PROGMEM  = {"Pulse '?' para menu de ayuda"};

#define ADDR_RTC   0x51  // Direccion I2C del RTC

#define RELE1      7
#define RELE2      8
#define RELE3      9
#define RELE4      10
#define DIN1       3
#define DIN2       4
#define DIN3       5
#define DIN4       6


byte reles[4] = {RELE1, RELE2, RELE3, RELE4};


void setup() 
{ 
  // Configura puerto I2c...
  Wire.begin();

  // Configura entradas y salidas...
  pinMode(RELE1, OUTPUT);  
  pinMode(RELE2, OUTPUT);  
  pinMode(RELE3, OUTPUT);  
  pinMode(RELE4, OUTPUT);  
  pinMode(DIN1, INPUT);    
  pinMode(DIN2, INPUT);    
  pinMode(DIN3, INPUT);      
  pinMode(DIN4, INPUT);        
  
  // Configura puerto serie...
  Serial.begin(9600); 
  
  // Inicializa la tarjeta SD...
  sdInit();

  
  // Mensaje inicial...
  sprint(pr20);
  sprint(pr21);
  Serial.println(); 
  sprint(pr22);
  Serial.println(); 
} 

void loop(){ 
  int c;
 
  // Comprueba lo que recibes por puerto serie....
  if (Serial.available() > 0) {
    int c = Serial.read();
    inString += (char)c; 
    if (c == 0x0D) {
      // Quita el retorno de carro...
      inString = inString.substring(0, inString.length() - 1);
      // comprueba el comando...
      // Menu...
      if(inString == "?"){
        menu();
      }
      // Rele ON...
      if(inString.startsWith("RO")){
        releOn(inString.charAt(2));
      }
      // Rele OFF...
      if(inString.startsWith("RF")){
        releOff(inString.charAt(2));
      }
      // Lee fecha y hora...
      if(inString.startsWith("RT")){
        timeR();
      }
       // Escribe hora...
      if(inString.startsWith("WT")){
        timeW(inString.substring(2));
      }
       // Escribe fecha...
      if(inString.startsWith("WD")){
        dateW(inString.substring(2));
      }
     
      // Lee analogicas...
      if(inString.startsWith("RA")){
        analogR();
      }
      // Lee digitales...
      if(inString.startsWith("RD")){
        digitalR();
      }

      inString = ""; 
    }    
  }

} 

void menu(){
  sprint(pr1);
  sprint(pr2);
  sprint(pr3);
  sprint(pr4);
  sprint(pr5);
//  sprint(pr6);
  sprint(pr7);
  sprint(pr8);
  sprint(pr9);
  sprint(pr10);

  sprint(pr2);  

}


void sprint (prog_uchar *pt){
  char buffer[100];
  
  strcpy_P(buffer, (char *)pt);
  Serial.println(buffer); 
}


void releOn(char rele){
  byte r;
  r = byte(rele) - 0x30;
  if(r > 8 || r < 1) return;
  digitalWrite(reles[r-1], HIGH);
  Serial.println("OK");
}

void releOff(char rele){
  byte r;
  r = byte(rele) - 0x30;
  if(r > 8 || r < 1) return;
  digitalWrite(reles[r-1], LOW);
  Serial.println("OK");  
}
void timeR(){
  Serial.println(rtc.formatDate(RTCC_DATE_WORLD));
  Serial.println(rtc.formatTime());
  Serial.println("OK");
}

void timeW(String mTime){
  rtc.setTime(mTime.substring(0,2).toInt(), mTime.substring(2,4).toInt(), mTime.substring(4,6).toInt());
  Serial.println("OK");
}

void dateW(String mDate){
  //day, weekday, month, century(1=1900, 0=2000), year(0-99)
  int dia = mDate.substring(0,2).toInt();
  int mes = mDate.substring(2,4).toInt();
  int ano = mDate.substring(4,6).toInt();
  rtc.setDate(dia, 1, mes, 0, ano);
  Serial.println("OK");
}


void analogR(){
  int an0, an1, an2, an3, an6, an7;
  float v0,v1,v2,v3,p1,p2;
  an0 = analogRead(0);  
  an1 = analogRead(1);  
  an2 = analogRead(2);    
  an3 = analogRead(3);      
  an6 = analogRead(6);    
  an7 = analogRead(7);      

  v0 = ((float)an0 * 10.0) / 1023.0;
  v1 = ((float)an1 * 10.0) / 1023.0;
  v2 = ((float)an2 * 10.0) / 1023.0;  
  v3 = ((float)an3 * 10.0) / 1023.0;    
  p1 = ((float)an6 * 100.0) / 1023.0;    
  p2 = ((float)an7 * 100.0) / 1023.0;    
  Serial.print("AN0: ");
  Serial.print(v0,1);
  Serial.println(" v");
  Serial.print("AN1: ");
  Serial.print(v1,1);
  Serial.println(" v");
  Serial.print("AN2: ");
  Serial.print(v2,1);
  Serial.println(" v");
  Serial.print("AN3: ");
  Serial.print(v3,1);
  Serial.println(" v");
  Serial.print("POT1: ");
  Serial.print(p1,0);
  Serial.println(" %");
  Serial.print("POT2: ");
  Serial.print(p2,0);
  Serial.println(" %");

  if(sdReady){
    logfile.print(rtc.formatTime());
    logfile.print(", ");
    logfile.print(p1,0);  
    logfile.print(", ");
    logfile.print(p2,0);  
    logfile.println();    
    logfile.flush();
  }
  
  Serial.println("OK");  
}

void digitalR(){
  Serial.print("DIN1 : ");
  if(digitalRead(DIN1) == LOW)
    Serial.println("ON");
  else
    Serial.println("OFF");

  Serial.print("DIN2 : ");
  if(digitalRead(DIN2) == LOW)
    Serial.println("ON");
  else
    Serial.println("OFF");

  Serial.print("DIN3 : ");
  if(digitalRead(DIN3) == LOW)
    Serial.println("ON");
  else
    Serial.println("OFF");
  Serial.print("DIN4 : ");
  if(digitalRead(DIN4) == LOW)
    Serial.println("ON");
  else
    Serial.println("OFF");
 
  Serial.println("OK");  

}

void error(char *str)
{
  Serial.print("error: ");
  Serial.println(str);
  
  // red LED indicates error
  while(1){
    digitalWrite(13, HIGH);
    delay(100);
    digitalWrite(13, LOW);
    delay(100);
  }

}

void sdInit (void){
  // initialize the SD card
  sdReady = true;
  Serial.print("Inicializando tarejta SD...");
  pinMode(chipSelect, OUTPUT);
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    sdReady = false;
    return;
  }
  Serial.println("Tarjeta inicializada.");
  
  // create a new file
  for (uint8_t i = 0; i < 100; i++) {
    filename[6] = i/10 + '0';
    filename[7] = i%10 + '0';
    if (! SD.exists(filename)) {
      // only open a new file if it doesn't exist
      logfile = SD.open(filename, FILE_WRITE); 
      break;  // leave the loop!
    }
  }
  
  if (! logfile) {
    error("No puedo crear el fichero");
  }
  
  Serial.print("Escribiendo en el fichero: ");
  Serial.println(filename);
  
  logfile.println("TIME, POT1, POT2");  
}


