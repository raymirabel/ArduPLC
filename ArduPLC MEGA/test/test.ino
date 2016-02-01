// Raimundo Alfonso Sanchez
// Ray Ingeniería Electrónica, S.L. - 2016
// Test ArduPLC MEGA

// Para usar ethernet, selecciona contolador W5500 dentro de la librería Ethernet.

#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>
#include <Wire.h>
#include <Rtc_Pcf8563.h>
#include "U8glib.h"
#include <ClickEncoder.h>
#include <TimerOne.h>
#include "PCF8574.h" 
#include "Logo_128x64.h"

// Mapa de pines ArduPLC MEGA - Arduino
#define RELE1   30
#define RELE2   31
#define RELE3   32
#define RELE4   33
#define RELE5   34
#define RELE6   35
#define DIN1    22
#define DIN2    23
#define DIN3    24
#define DIN4    25
#define DIN5    26
#define DIN6    27
#define DIN7    28
#define DIN8    29
#define AIN1    A6
#define AIN2    A7
#define AIN3    A8
#define AIN4    A9
#define AIN5    A10
#define AIN6    A11
#define POT1    A14
#define POT2    A15
#define AOUT1   44
#define AOUT2   45
#define LED1    36
#define LED2    37
#define WIRE    38
#define BUZZER  49
#define CS_uSD      46
#define INSERT_uSD  47

// Driver para OLED, azul = SH1106, blanco = SSD1306
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE|U8G_I2C_OPT_DEV_0);  // I2C / TWI 
//U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_DEV_0|U8G_I2C_OPT_NO_ACK|U8G_I2C_OPT_FAST); // Fast I2C / TWI 
//U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);  // Display which does not send AC
//U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_NONE);  // I2C / TWI 
//U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_DEV_0|U8G_I2C_OPT_FAST); // Dev 0, Fast I2C / TWI
//U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_NO_ACK); // Display which does not send ACK

// Ethernet...
#if defined(WIZ550io_WITH_MACADDRESS) // Use assigned MAC address of WIZ550io
;
#else
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
#endif  
IPAddress ip(192,168,1,190);
EthernetServer server(80);

// uSD...
Sd2Card card;
SdVolume volume;
SdFile root;

// rtc...
#define ADDR_RTC   0x51  // Direccion I2C del RTC
Rtc_Pcf8563 rtc;  

// Encoder...
// CHA, CHB, ENTER, DIV
ClickEncoder encoder(18, 15, 14, 4);
void timerIsr() {
  encoder.service();
}

// I2C expander...
PCF8574 expander;


void menu_inicio(void){
  Serial.println(F("---    Test ArduPLC MEGA v1.00    ---"));
  Serial.println(F("   Ray Ingenieria Electronica,S.L."));  
  Serial.println();
  Serial.println(F("Pulse '?' para menu de ayuda"));    
}

void menu(void){
  Serial.println(F("                MENU PRINCIPAL"));
  Serial.println(F("-----------------------------------------------"));
  Serial.println(F("Rn       Activa rele n (n de 1 a 6)"));
  Serial.println(F("Fn       Desactiva rele n (n de 1 a 6)"));
  Serial.println(F("D        Lee entradas digitales"));
  Serial.println(F("1        Lee entrada 1-WIRE"));  
  Serial.println(F("A        Lee entradas analogicas"));
  Serial.println(F("Onxxxxx  Salida analogica n (n de 1 a 2, xxxxx = mV)"));
  Serial.println(F("RT       Lee fecha y hora (solo con RTC)"));  
  Serial.println(F("WDddmmaa Escribe fecha en el RTC"));
  Serial.println(F("WThhmmss Escribe hora en el RTC"));
  Serial.println(F("B        Test L1, L2 y buzzer"));  
  Serial.println(F("S        Test uSD"));    
  Serial.println(F("E        Lee entradas digitales modulo expansion"));  
            
}

String inString;
int aout1 = 0;
int aout2 = 0;
int param = 0;


void setup(){ 
  pinMode(RELE1, OUTPUT);
  pinMode(RELE2, OUTPUT);
  pinMode(RELE3, OUTPUT);
  pinMode(RELE4, OUTPUT);
  pinMode(RELE5, OUTPUT);
  pinMode(RELE6, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(DIN1, INPUT_PULLUP);
  pinMode(DIN2, INPUT_PULLUP);
  pinMode(DIN3, INPUT_PULLUP);
  pinMode(DIN4, INPUT_PULLUP);
  pinMode(DIN5, INPUT_PULLUP);
  pinMode(DIN6, INPUT_PULLUP);
  pinMode(DIN7, INPUT_PULLUP);    
  pinMode(DIN8, INPUT_PULLUP);  
  pinMode(WIRE, INPUT);

  pinMode(CS_uSD, OUTPUT);
  pinMode(INSERT_uSD, INPUT);  
  digitalWrite(CS_uSD, HIGH); // Deshabilita uSD para no interferir con el controlador W5500

  // Configura OLED...
  u8g.begin();
  u8g.setFont(u8g_font_6x13r);
  u8g.setFontRefHeightText();
  u8g.setFontPosTop();

  // Visualiza logo en pantalla OLED...
  u8g.firstPage();  
  do {
    u8g.drawBitmapP( 0, 0, 16, 64, logo_ray);
  } while( u8g.nextPage() );  
  
  
  // Referencia externa
  analogReference(EXTERNAL);
  
  // Configura puertos serie...
  Serial.begin(9600); 
  Serial2.begin(9600); 

  // Inicializa ethernet y servidor web...
#if defined(WIZ550io_WITH_MACADDRESS)
  Ethernet.begin(ip);
#else
  Ethernet.begin(mac, ip);
#endif  
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());


  // Inicializa y configura módulo expansión...
  // Comenta esta linea si no tienes conectado el módulo de expansión...
  //expander.begin(0x20);

  // Espera un poco mostrando el logo...
  delay(3000);

  // Mensaje inicial...
  menu_inicio();

  // Menu de opciones...
  menu();

  // Timer cada 1ms para el encoder...
  Timer1.initialize(1000);
  Timer1.attachInterrupt(timerIsr); 
} 

void loop(){ 
  int c;
  static long timer_oled = 0;


  // HMI...
  gestiona_encoder();
  if((millis() - timer_oled) >= 100){
    timer_oled = millis();
    gestiona_oled();
  }

  // Servidor web...
  test_server();
  
  // Comprueba lo que recibes por puerto RS485....
  if (Serial2.available() > 0) {
    int c = Serial2.read();
    inString += (char)c; 
    if (c == 0x0D) {
      // Quita el retorno de carro...
      inString = inString.substring(0, inString.length() - 1);
      // comprueba el comando...
      // Menu...
      if(inString == "?"){
        Serial2.println(F("             MENU PRINCIPAL POR RS485 "));
        Serial2.println(F("-----------------------------------------------"));
        Serial2.println(F("Rn       Activa rele n (n de 1 a 6)"));
        Serial2.println(F("Fn       Desactiva rele n (n de 1 a 6)"));
      }
      // Rele ON...
      if(inString.startsWith("R")){
        releOn(inString.charAt(1));
        Serial2.println("OK");
      }
      // Rele OFF...
      if(inString.startsWith("F")){
        releOff(inString.charAt(1));
        Serial2.println("OK");        
      }         
      inString = ""; 
    }  
  }
  // Comprueba lo que recibes por puerto USB....
  if (Serial.available() > 0) {
    int c = Serial.read();
    inString += (char)c; 
    if (c == 0x0D) {
      // Quita el retorno de carro...
      //inString = inString.substring(0, inString.length() - 1);
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
      // Lee entradas digitales...
      if(inString.startsWith("D")){
        digitalR();
        Serial.println("OK");        
      }
      // Lee entradas 1-wire...
      if(inString.startsWith("1")){
        digitalWire();
        Serial.println("OK");        
      }      
      // Salidas analógicas...
      if(inString.startsWith("O")){
        analogOut(inString);
        Serial.println("OK");
      }      
      // Test leds y buzzer...
      if(inString.startsWith("B")){
        leds();
        Serial.println("OK");        
      }      
      // Test uSD...
      if(inString.startsWith("S")){
        test_sd();
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
      if(inString.startsWith("E")){
        Serial.println("Exp digital inputs: ");      
        Serial.println(!expander.digitalRead(0),DEC);      
        Serial.println(!expander.digitalRead(1),DEC);  
        Serial.println(!expander.digitalRead(2),DEC);  
        Serial.println(!expander.digitalRead(3),DEC);        
        Serial.println("OK");      
     }       
    
      inString = ""; 
      Serial.flush();
    }    
  }

  // Actualiza salidas analógicas...
  analogWrite(AOUT1, aout1); 
  analogWrite(AOUT2, aout2); 

} 


void releOn(char rele){
  byte r;
  r = byte(rele) - 0x30;
  if(r == 1) digitalWrite(RELE1, HIGH);
  if(r == 2) digitalWrite(RELE2, HIGH);  
  if(r == 3) digitalWrite(RELE3, HIGH);  
  if(r == 4) digitalWrite(RELE4, HIGH);  
  if(r == 5) digitalWrite(RELE5, HIGH);  
  if(r == 6) digitalWrite(RELE6, HIGH);      
}

void releOff(char rele){
  byte r;
  r = byte(rele) - 0x30;
  if(r == 1) digitalWrite(RELE1, LOW);
  if(r == 2) digitalWrite(RELE2, LOW);  
  if(r == 3) digitalWrite(RELE3, LOW);  
  if(r == 4) digitalWrite(RELE4, LOW);  
  if(r == 5) digitalWrite(RELE5, LOW);  
  if(r == 6) digitalWrite(RELE6, LOW);  
}

void analogR(){
  int an1, an2, an3, an4, an5, an6, an7, an8, an9;
  float v1, v2, v3, v4, v5, v6, pot1, pot2;
  an1 = analogRead(AIN1);  
  an2 = analogRead(AIN2);
  an3 = analogRead(AIN3);    
  an4 = analogRead(AIN4);  
  an5 = analogRead(AIN5);  
  an6 = analogRead(AIN6);  
  an7 = analogRead(POT1);  
  an8 = analogRead(POT2);    
  
  
  v1 = ((float)an1 * 10.0) / 1023.0;
  v2 = ((float)an2 * 10.0) / 1023.0;
  v3 = ((float)an3 * 10.0) / 1023.0;  
  v4 = ((float)an4 * 10.0) / 1023.0;  
  v5 = ((float)an5 * 10.0) / 1023.0;  
  v6 = ((float)an6 * 10.0) / 1023.0;  
  pot1 = ((float)an7 * 100.0) / 1023.0;  
  pot2 = ((float)an8 * 100.0) / 1023.0;        
 
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
  Serial.print("POT1: ");
  Serial.print(pot1,0);
  Serial.println(" %");
  Serial.print("POT2: ");
  Serial.print(pot2,0);
  Serial.println(" %");     
}

void digitalR(){
  Serial.print("DIN1 : ");
  Serial.println(!digitalRead(DIN1));

  Serial.print("DIN2 : ");
  Serial.println(!digitalRead(DIN2));

  Serial.print("DIN3 : ");
  Serial.println(!digitalRead(DIN3));
  
  Serial.print("DIN4 : ");
  Serial.println(!digitalRead(DIN4));

  Serial.print("DIN5 : ");
  Serial.println(!digitalRead(DIN5));

  Serial.print("DIN6 : ");
  Serial.println(!digitalRead(DIN6));

  Serial.print("DIN7 : ");
  Serial.println(!digitalRead(DIN7));

  Serial.print("DIN8 : ");
  Serial.println(!digitalRead(DIN8));
}

void digitalWire(){
  Serial.print("1-WIRE PIN : ");
  Serial.println(digitalRead(WIRE));
}

void analogOut(String s){
  byte r;
  char out;
  int mv;
  
  String s1;
  
  out = s.charAt(1);
  s1 = s.substring(2);
  mv = s1.toInt();
  r = byte(out) - 0x30;
  if(r == 1){
    aout1 = (((long)mv * 255L) / 10000L); 
  }
  if(r == 2){
    aout2 = (((long)mv * 255L) / 10000L); 
  } 
}

void leds(void){
  digitalWrite(BUZZER, HIGH);
  delay(500);
  digitalWrite(BUZZER, LOW);
  digitalWrite(LED1, HIGH);
  delay(500);
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, HIGH);
  delay(500);
  digitalWrite(LED2, LOW);
}

void test_server(){
  int an1, an2, an3, an4, an5, an6, an7, an8, an9;
  float v1, v2, v3, v4, v5, v6, pot1, pot2;
  
 // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          // output the value of each analog input pin

          an1 = analogRead(AIN1);  
          an2 = analogRead(AIN2);
          an3 = analogRead(AIN3);    
          an4 = analogRead(AIN4);  
          an5 = analogRead(AIN5);  
          an6 = analogRead(AIN6);  
          an7 = analogRead(POT1);  
          an8 = analogRead(POT2);    
          v1 = ((float)an1 * 10.0) / 1023.0;
          v2 = ((float)an2 * 10.0) / 1023.0;
          v3 = ((float)an3 * 10.0) / 1023.0;  
          v4 = ((float)an4 * 10.0) / 1023.0;  
          v5 = ((float)an5 * 10.0) / 1023.0;  
          v6 = ((float)an6 * 10.0) / 1023.0;  
          pot1 = ((float)an7 * 100.0) / 1023.0;  
          pot2 = ((float)an8 * 100.0) / 1023.0;        
          client.print("AN1: ");
          client.print(v1,3);
          client.println(" v");
          client.println("<br />");               
          client.print("AN2: ");
          client.print(v2,3);
          client.println(" v");
          client.println("<br />");               
          client.print("AN3: ");
          client.print(v3,3);
          client.println(" v");
          client.println("<br />");               
          client.print("AN4: ");
          client.print(v4,3);
          client.println(" v");
          client.println("<br />");               
          client.print("AN5: ");
          client.print(v5,3);
          client.println(" v");
          client.println("<br />");               
          client.print("AN6: ");
          client.print(v6,3);
          client.println(" v");
          client.println("<br />");               
          client.print("POT1: ");
          client.print(pot1,0);
          client.println(" %");
          client.println("<br />");               
          client.print("POT2: ");
          client.print(pot2,0);
          client.println(" %");  
          client.println("<br />");               
          
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disonnected");
  }
}

void test_sd(void){
   // we'll use the initialization code from the utility libraries
  // since we're just testing if the card is working!
  if(digitalRead(INSERT_uSD) == 1){
    Serial.println("uSD no preset.");
    return;
  }
  if (!card.init(SPI_HALF_SPEED, CS_uSD)) {
    Serial.println("initialization failed. Things to check:");
    Serial.println("* is a card inserted?");
    Serial.println("* is your wiring correct?");
    Serial.println("* did you change the chipSelect pin to match your shield or module?");
    return;
  } else {
    Serial.println("Wiring is correct and a card is present.");
  }

  // print the type of card
  Serial.print("\nCard type: ");
  switch (card.type()) {
    case SD_CARD_TYPE_SD1:
      Serial.println("SD1");
      break;
    case SD_CARD_TYPE_SD2:
      Serial.println("SD2");
      break;
    case SD_CARD_TYPE_SDHC:
      Serial.println("SDHC");
      break;
    default:
      Serial.println("Unknown");
  }

  // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
  if (!volume.init(card)) {
    Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
    return;
  }


  // print the type and size of the first FAT-type volume
  uint32_t volumesize;
  Serial.print("\nVolume type is FAT");
  Serial.println(volume.fatType(), DEC);
  Serial.println();

  volumesize = volume.blocksPerCluster();    // clusters are collections of blocks
  volumesize *= volume.clusterCount();       // we'll have a lot of clusters
  volumesize *= 512;                            // SD card blocks are always 512 bytes
  Serial.print("Volume size (bytes): ");
  Serial.println(volumesize);
  Serial.print("Volume size (Kbytes): ");
  volumesize /= 1024;
  Serial.println(volumesize);
  Serial.print("Volume size (Mbytes): ");
  volumesize /= 1024;
  Serial.println(volumesize);


  Serial.println("\nFiles found on the card (name, date and size in bytes): ");
  root.openRoot(volume);

  // list all files in the card with date and size
  root.ls(LS_R | LS_DATE | LS_SIZE);
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

void gestiona_encoder(void){
  int enc;
  static long posSP  = 0;
  
  enc = encoder.getValue();  
  if(enc != 0){
      switch(param){
        case 0:
          aout1 -= enc;
          if(aout1 < 0) aout1 = 0;
          if(aout1 > 255) aout1 = 255;
          break;
        case 1:
          aout2 -= enc;
          if(aout2 < 0) aout2 = 0;
          if(aout2 > 255) aout2 = 255;          
          break;
      }
  }
  
  if(encoder.getButton() == ClickEncoder::Clicked){
    param ^= 0x01;
  }
}

void gestiona_oled(void){
  int h, w, temp, n;
  char sTemp[30];

  
  u8g.firstPage();
  do{
    u8g.setFont(u8g_font_5x8r);  
    u8g.setFontRefHeightText();
    u8g.setFontPosTop();
    h = u8g.getFontAscent()-u8g.getFontDescent();
    w = u8g.getWidth();
    
    // Visualiza entradas analógicas...
    sprintf(sTemp, "AI1:%4d",analogRead(AIN1));      
    u8g.drawStr(1, 0*h-1 , sTemp);
    sprintf(sTemp, "AI2:%4d",analogRead(AIN2));       
    u8g.drawStr(1, 1*h-1 , sTemp);      
    sprintf(sTemp, "AI3:%4d",analogRead(AIN3));       
    u8g.drawStr(1, 2*h-1 , sTemp);   
    sprintf(sTemp, "AI4:%4d",analogRead(AIN4));       
    u8g.drawStr(1, 3*h-1 , sTemp);   
    sprintf(sTemp, "AI5:%4d",analogRead(AIN5));       
    u8g.drawStr(1, 4*h-1 , sTemp); 
    sprintf(sTemp, "AI6:%4d",analogRead(AIN6));       
    u8g.drawStr(1, 5*h-1 , sTemp);   
    
    if(param == 0){
      u8g.setDefaultForegroundColor();    
      u8g.drawBox(1, 6*h, 40, 6); 
      u8g.setDefaultBackgroundColor();   
    }
    sprintf(sTemp, "AO1:%4d",aout1);       
    u8g.drawStr(1, 6*h-1 , sTemp);   
    u8g.setDefaultForegroundColor();

    if(param == 1){
      u8g.setDefaultForegroundColor();    
      u8g.drawBox(1, 7*h, 40, 6); 
      u8g.setDefaultBackgroundColor();   
    }
    sprintf(sTemp, "AO2:%4d",aout2);       
    u8g.drawStr(1, 7*h-1 , sTemp);   
    u8g.setDefaultForegroundColor();

    
    u8g.drawLine(0,56,127,56);
    sprintf(sTemp, "%s - %s",rtc.formatDate(RTCC_DATE_WORLD), rtc.formatTime());     
    u8g.drawStr(12, 8*h+1, sTemp);

    u8g.setFont(u8g_font_6x10r); 
    u8g.setFontRefHeightText();
    u8g.setFontPosTop();      
    h = u8g.getFontAscent()-u8g.getFontDescent();
    w = u8g.getWidth();      
    
    // Visualiza potenciometros...
    n = (int)((analogRead(POT1)*100L) / 1023L);
    sprintf(sTemp, "POT1:%3d%%",n);       
    u8g.drawStr(50, 2*h-1 , sTemp);   
    n = (int)((analogRead(POT2)*100L) / 1023L);
    sprintf(sTemp, "POT2:%3d%%",n);       
    u8g.drawStr(50, 3*h-1 , sTemp);   


    // Visualiza entradas digitales...
    u8g.drawStr(50, 1*h-1 , "DIN: 87654321"); 
    for(n=0;n<=7;n++){
      //if(!digitalRead(DIN8-n)) u8g.drawStr(50+(6*(5+n)), 0*h-1 , "#"); 
      if(!digitalRead(DIN8-n))
        u8g.drawBox(51+(6*(5+n)),2,2,4);
      u8g.drawFrame(49+(6*(5+n)),0,6,8);
    }
    
    // Visualiza salidas rele...      
    u8g.drawStr(50, 4*h-1 , "RELAY: 123456");   
    for(n=0;n<=5;n++){
      //if(digitalRead(RELE1+n)) u8g.drawStr(50+(6*(7+n)), 5*h-1 , "#"); 
      if(digitalRead(RELE1+n))
        u8g.drawBox(51+(6*(7+n)),46,2,4);
      u8g.drawFrame(49+(6*(7+n)),44,6,8);        
    }          

  }while(u8g.nextPage());
  
}


