// Raimundo Alfonso Sanchez
// Ray Ingeniería Electrónica, S.L. - 2015
// Test ArduPLC MICRO

#include <Wire.h>
#include <Rtc_Pcf8563.h>

#define RELE1   4
#define RELE2   5
#define RELE3   6
#define RELE4   7
#define DIN1    0
#define DIN2    1
#define DIN3    9
#define DIN4    10
#define DIN5    11
#define DIN6    13
#define AIN1    A0
#define AIN2    A1
#define AIN3    A2
#define AIN4    A5
#define AIN5    A4
#define AIN6    A3
#define LED     12

#define CAL_5V  5.000    // Para mayor precisión en la medida de entradas analógicas, mide el voltaje en la borna auxiliar de 5V y copialo aquí.

 
#define ADDR_RTC   0x51  // Direccion I2C del RTC
Rtc_Pcf8563 rtc;  


void menu_inicio(void){
  Serial.println(F("---    Test placa ArduPLC MICRO    ---"));
  Serial.println(F("   Ray Ingenieria Electronica, S.L."));  
  Serial.println();
  Serial.println(F("Pulse '?' para menu de ayuda"));    
}

void menu(void){
  Serial.println(F("                MENU PRINCIPAL"));
  Serial.println(F("-----------------------------------------------"));
  Serial.println(F("Rn       Activa rele n (n de 1 a 4)"));
  Serial.println(F("Fn       Desactiva rele n (n de 1 a 4)"));
  Serial.println(F("D        Lee entradas digitales"));
  Serial.println(F("A        Lee entradas analogicas (10V)"));
  Serial.println(F("S        Lee entradas analogicas (20mA)"));
  Serial.println(F("L        Test led L"));  
  Serial.println(F("RT       Lee fecha y hora (solo con RTC)"));
  Serial.println(F("WDddmmaa Escribe fecha en el RTC (solo con RTC)"));
  Serial.println(F("WThhmmss Escribe hora en el RTC (solo con RTC)"));  
}

String inString;


void setup(){ 
  pinMode(RELE1, OUTPUT);
  pinMode(RELE2, OUTPUT);
  pinMode(RELE3, OUTPUT);
  pinMode(RELE4, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(DIN1, INPUT);
  pinMode(DIN2, INPUT);
  pinMode(DIN3, INPUT);
  pinMode(DIN4, INPUT);
  pinMode(DIN5, INPUT);
  pinMode(DIN6, INPUT);

  delay(4000);

  // Configura puertos serie...
  Serial.begin(9600); 

  // Mensaje inicial...
  menu_inicio();

  // Menu de opciones...
  menu();
} 

void loop(){ 
  int c;

  // Comprueba lo que recibes por puerto serie....
  if (Serial.available() > 0) {
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
      // Lee entradas analogicas...
      if(inString.startsWith("A")){
        analogR();
        Serial.println("OK");        
      }  
      // Lee entradas analogicas mA...
      if(inString.startsWith("S")){
        analogCurrent();
        Serial.println("OK");        
      }       
      // Lee entradas digitales...
      if(inString.startsWith("D")){
        digitalR();
        Serial.println("OK");        
      }
      // Test leds y buzzer...
      if(inString.startsWith("L")){
        led();
        Serial.println("OK");        
      }      
      // Lee fecha y hora...
      if(inString.startsWith("RT")){
        Serial.println(leeFecha());
        Serial.println(leeHora());        
        Serial.println("OK");
      }
       // Escribe hora...
      if(inString.startsWith("WT")){
        escribeHora(inString.substring(2));
        Serial.println("OK");        
      }
       // Escribe fecha...
      if(inString.startsWith("WD")){
        escribeFecha(inString.substring(2));
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
  if(r == 1) digitalWrite(RELE1, HIGH);
  if(r == 2) digitalWrite(RELE2, HIGH);  
  if(r == 3) digitalWrite(RELE3, HIGH);  
  if(r == 4) digitalWrite(RELE4, HIGH);  
}

void releOff(char rele){
  byte r;
  r = byte(rele) - 0x30;
  if(r == 1) digitalWrite(RELE1, LOW);
  if(r == 2) digitalWrite(RELE2, LOW);  
  if(r == 3) digitalWrite(RELE3, LOW);  
  if(r == 4) digitalWrite(RELE4, LOW);  
}

void analogR(){
  int an1, an2, an3, an4, an5, an6;
  float v1, v2, v3, v4, v5, v6;
  an1 = analogRead(AIN1);  
  an2 = analogRead(AIN2);
  an3 = analogRead(AIN3);    
  an4 = analogRead(AIN4);  
  an5 = analogRead(AIN5);  
  an6 = analogRead(AIN6);  
  
  v1 = ((float)an1 * 10.0) / 1023.0;
  v2 = ((float)an2 * 10.0) / 1023.0;
  v3 = ((float)an3 * 10.0) / 1023.0;  
  v4 = ((float)an4 * 10.0) / 1023.0;  
  v5 = ((float)an5 * 10.0) / 1023.0;  
  v6 = ((float)an6 * 10.0) / 1023.0;  

  v1 *= (CAL_5V/5.000);
  v2 *= (CAL_5V/5.000);
  v3 *= (CAL_5V/5.000);
  v4 *= (CAL_5V/5.000);
  v5 *= (CAL_5V/5.000);
  v6 *= (CAL_5V/5.000);  
 
  Serial.print("AN1: ");
  Serial.print(v1,3);
  Serial.println(" v");
  Serial.print("AN2: ");
  Serial.print(v2,3);
  Serial.println(" v");
  Serial.print("AN3: ");
  Serial.print(v3,3);
  Serial.println(" v");
  Serial.print("AN4: ");
  Serial.print(v4,3);
  Serial.println(" v");
  Serial.print("AN5: ");
  Serial.print(v5,3);
  Serial.println(" v");
  Serial.print("AN6: ");
  Serial.print(v6,3);
  Serial.println(" v");
}

void analogCurrent(){
  int an1, an2, an3, an4, an5, an6;
  float i1, i2, i3, i4, i5, i6;
  an1 = analogRead(AIN1);  
  an2 = analogRead(AIN2);
  an3 = analogRead(AIN3);    
  an4 = analogRead(AIN4);  
  an5 = analogRead(AIN5);  
  an6 = analogRead(AIN6);  
  
  i1 = ((float)an1 * 20.0) / 1023.0;
  i2 = ((float)an2 * 20.0) / 1023.0;
  i3 = ((float)an3 * 20.0) / 1023.0;  
  i4 = ((float)an4 * 20.0) / 1023.0;  
  i5 = ((float)an5 * 20.0) / 1023.0;  
  i6 = ((float)an6 * 20.0) / 1023.0;  
  
  i1 *= (CAL_5V/5.000);
  i2 *= (CAL_5V/5.000);
  i3 *= (CAL_5V/5.000);
  i4 *= (CAL_5V/5.000);
  i5 *= (CAL_5V/5.000);
  i6 *= (CAL_5V/5.000);  
 
  Serial.print("AN1: ");
  Serial.print(i1,2);
  Serial.println(" mA");
  Serial.print("AN2: ");
  Serial.print(i2,2);
  Serial.println(" mA");
  Serial.print("AN3: ");
  Serial.print(i3,2);
  Serial.println(" mA");
  Serial.print("AN4: ");
  Serial.print(i4,2);
  Serial.println(" mA");
  Serial.print("AN5: ");
  Serial.print(i5,2);
  Serial.println(" mA");
  Serial.print("AN6: ");
  Serial.print(i6,2);
  Serial.println(" mA");
}

void digitalR(){
  Serial.print("DIN1 : ");
  Serial.println(digitalRead(DIN1));

  Serial.print("DIN2 : ");
  Serial.println(digitalRead(DIN2));

  Serial.print("DIN3 : ");
  Serial.println(digitalRead(DIN3));
  
  Serial.print("DIN4 : ");
  Serial.println(digitalRead(DIN4));

  Serial.print("DIN5 : ");
  Serial.println(digitalRead(DIN5));

  Serial.print("DIN6 : ");
  Serial.println(digitalRead(DIN6));
}


void led(void){
  digitalWrite(LED, HIGH);
  delay(1000);
  digitalWrite(LED, LOW);
}

String leeFecha(void){
  return(rtc.formatDate(RTCC_DATE_WORLD));
}

String leeHora(void){
  return(rtc.formatTime());
}

void escribeFecha(String mDate){
  //day, weekday, month, century(1=1900, 0=2000), year(0-99)
  int dia = mDate.substring(0,2).toInt();
  int mes = mDate.substring(2,4).toInt();
  int ano = mDate.substring(4,6).toInt();
  rtc.setDate(dia, 1, mes, 0, ano);
}
  
void escribeHora(String mTime){
  rtc.setTime(mTime.substring(0,2).toInt(), mTime.substring(2,4).toInt(), mTime.substring(4,6).toInt());
}


