/**************************************************************************
  @FILE:         test_ArduPLC_V2_HMI.ino
  @AUTHOR:       Santiago Pérez Tello
  @COMPANY:      Ray Ingeniería Electronica, S.L.
  @DESCRIPTION:  Test de ArduPLC V2.00 con HMI CNMB5_I2C para ArduPLCs

  @LICENCE DETAILS:
  Este sketch está basada en software libre. Tu puedes redistribuir
  y/o modificar esta sketch bajo los términos de licencia GNU.

  Esta programa se distribuye con la esperanza de que sea útil,
  pero SIN NINGUNA GARANTÍA, incluso sin la garantía implícita de
  COMERCIALIZACIÓN O PARA UN PROPÓSITO PARTICULAR.
  Consulte los términos de licencia GNU para más detalles:

  http://www.gnu.org/licenses/gpl-3.0.txt

  This sketch is based on free software. You can redistribute
  and/or modify this library under the terms of the GNU license.

  This software is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY, even without the implied warranty of
  MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU license terms for more details:

  http://www.gnu.org/licenses/gpl-3.0.txt

  @VERSIONS:
  08-01-2019 - v1.00 : Primera versión. Incluir comentarios acerca de la nueva versión

**************************************************************************/

// Libreias necesarias
#include  <Wire.h>        // Control del bus I2C
#include  "PCF8574.h"     // Control del expansor I/O de 8 puertos por I2C
#include  "U8glib.h"      // Control de pantalla
#include  <Rtc_Pcf8563.h> // Control de Reloj en Tiempo Real
#include  "logo_ray.h"    // Logotipo de Ray Ingeniería Electrónica
#include  <SD.h>          // Control de tarjeta SD
#include  <SPI.h>         // Control del bus ISP, necesario para comunicación con tarjeta SD

// Mapa de pines ArduPLC V2.00
#define RELE1   7
#define RELE2   8
#define RELE3   9
#define RELE4   10
#define DIN1    3
#define DIN2    4
#define DIN3    5
#define DIN4    6
#define AIN1    A0
#define AIN2    A1
#define AIN3    A2
#define AIN4    A3
#define POT1    A6
#define POT2    A7
#define LEDBUZZ 13
#define CS_uSD  2       // Chip Select uSD

// Asignamos una palabra clave al pin al que corresponde cada botón
#define BT_UP       0   // Boton up conectado al pin 0 del PCF8574T
#define BT_DOWN     1   // Boton down conectado al pin 1 del PCF8574T
#define BT_LEFT     2   // Boton left conectado al pin 2 del PCF8574T
#define BT_RIGHT    3   // Boton RIGHT conectado al pin 3 del PCF8574T
#define BT_INTRO    4   // Boton intro conectado al pin 4 del PCF8574T

// Asignamos una palabra clave al pin al que corresponde cada led
#define LED_RED     5   // Led rojo conectado al P5 del PCF8574T
#define LED_GRE     6   // Led verde conectado al P6 del PCF8574T
#define LED_ORA     7   // Led naranja conectado al P7 del PCF8574T

// Instancia del expansor I2C
PCF8574 HMI_IO;

// Instancia de la pantalla OLED
U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_DEV_0 | U8G_I2C_OPT_FAST);

// Instancia para la tarjeta uSD
/*Sd2Card card;
  SdVolume volume;
  SdFile root;*/

// Instancia y direccion del Real Time Clock (RTC)
#define ADDR_RTC  0x51
Rtc_Pcf8563 rtc;


/*******************************************************************************
  SETUP (solo se ejecuta una vez)
*******************************************************************************/
void setup() {
  // Configuración de Relés
  pinMode(RELE1, OUTPUT);
  pinMode(RELE2, OUTPUT);
  pinMode(RELE3, OUTPUT);
  pinMode(RELE4, OUTPUT);

  // Configuración de las entradas digitales
  pinMode(DIN1, INPUT);
  pinMode(DIN2, INPUT);
  pinMode(DIN3, INPUT);
  pinMode(DIN4, INPUT);

  // Configuracion de la tarjeta uSD
  pinMode (CS_uSD, OUTPUT);
  digitalWrite (CS_uSD, HIGH);

  delay(200);
  Serial.begin(9600);

  // Inicializamos el expansor I2C con la dirección 0x20
  // Podemos modificar físicamente esta dirección si tenemos conflicto con otro expanson en el mismo bus.
  // Para ello debes modificar el estado de los jumpers S7, S8 y S9 (A0, A1 y A2) que hay en la parte posterior de la PCB.
  // Consulta el manual de la placa CNMB5_I2C para mas detalles.
  ErrorI2c();
  HMI_IO.begin(0x20);

  // Indicamos qué pines del expansor I2C son de entrada (BOTONES) LOW = Pulsado,  HIGH = NO Pulsado
  HMI_IO.pinMode(BT_UP, INPUT_PULLUP);
  HMI_IO.pinMode(BT_DOWN, INPUT_PULLUP);
  HMI_IO.pinMode(BT_LEFT, INPUT_PULLUP);
  HMI_IO.pinMode(BT_RIGHT, INPUT_PULLUP);
  HMI_IO.pinMode(BT_INTRO, INPUT_PULLUP);

  // Indicamos qué pines del expansor I2C son de salida (LEDs)
  HMI_IO.pinMode(LED_RED, OUTPUT);
  HMI_IO.pinMode(LED_GRE, OUTPUT);
  HMI_IO.pinMode(LED_ORA, OUTPUT);

  // Indicamos que los leds estén apagados inicialmente
  // (LOW = Encendido y HIGH = APAGADO)
  HMI_IO.digitalWrite(LED_RED, HIGH);
  HMI_IO.digitalWrite(LED_GRE, HIGH);
  HMI_IO.digitalWrite(LED_ORA, HIGH);

  // Dibujamos logo de Ray Ingenieria Electrónica
  DrawRayLogo();
  delay(4000);

  MenuInicio();
  Menu();
}

/*******************************************************************************
  LOOP (se ejecuta continuamente)
*******************************************************************************/
void loop() {
  static String inString;


    GestionaOled();
  

  // Comprueba lo que recibes por puerto USB
  if (Serial.available() > 0) {
    int c = Serial.read();
    inString += (char)c;
    if (c == 0x0D) {
      // Quita el retorno de carro
      inString.trim();
      // Comprueba el comando introducido
      if (inString == "?") {
        Menu();
      }
      // Rele ON
      if (inString.startsWith(F("R"))) {
        ReleOn(inString.charAt(1));
        EscribirOk();
      }
      // Rele OFF
      if (inString.startsWith(F("F"))) {
        ReleOff(inString.charAt(1));
        EscribirOk();
      }
      // Lee entradas analogicas
      if (inString.startsWith(F("A"))) {
        AnalogR();
        EscribirOk();
      }
      // Lee entradas digitales
      if (inString.startsWith(F("D"))) {
        DigitalR();
        EscribirOk();
      }

      // Test leds de expansor
      if (inString.startsWith(F("B"))) {
        ErrorI2c();
        LedsHmi();
        EscribirOk();
      }

      if (inString.startsWith(F("L"))) {
        LedBuzzer();
        EscribirOk();
      }

      // Test uSD
      /*if (inString.startsWith(F("S"))) {
        test_sd();
        EscribirOk();
        }*/

      // Lee fecha y hora
      if (inString.startsWith(F("RT"))) {
        Serial.println( LeeFecha() );
        Serial.println( LeeHora() );
        EscribirOk();
      }
      // Escribe hora
      if (inString.startsWith(F("WT"))) {
        EscribeHora(inString.substring(2));
        EscribirOk();
      }
      // Escribe fecha
      if (inString.startsWith(F("WD"))) {
        EscribeFecha(inString.substring(2));
        EscribirOk();
      }

      // Lee botones expansor
      if (inString.startsWith(F("E"))) {
        ErrorI2c();
        BotonesHmi();
        EscribirOk();
      }

      inString = "";
      Serial.println();
      Serial.flush();
    }

  }
}



/*******************************************************************************
  FUNCIONES IMPLEMENTADAS
*******************************************************************************/
void MenuInicio(void) {
  Serial.println(F("    ---   Test ArduPLC V2.00 HMI   ---"));
  Serial.println(F("       Ray Ingenieria Electronica,S.L."));
  Serial.println(F("        Pulse '?' para menu de ayuda \n"));
}

void Menu(void) {
  Serial.println(F("              MENU PRINCIPAL"));
  Serial.println(F("-----------------------------------------------"));
  Serial.println(F("Rn       Activa relé n (n de 1 a 4)"));
  Serial.println(F("Fn       Desactiva relé n (n de 1 a 4)"));
  Serial.println(F("D        Lee entradas digitales"));
  Serial.println(F("A        Lee entradas analógicas"));
  Serial.println(F("RT       Lee fecha y hora (solo con RTC)"));
  Serial.println(F("WDddmmaa Escribe fecha en el RTC"));
  Serial.println(F("WThhmmss Escribe hora en el RTC"));
  Serial.println(F("B        Test leds HMI"));
  Serial.println(F("E        Lee pulsadores HMI "));
  Serial.println(F("L        Test Led interno y buzzer"));
  Serial.println(F("S        Test uSD"));

}

void DrawRayLogo () {
  static int i = -129;
  u8g.setFont(u8g_font_9x15Br);
  u8g.setFontRefHeightText();
  u8g.setFontPosTop();

  while (i <= 0) {
    u8g.firstPage();
    do {
      u8g.drawBitmapP( 0 , i, 16, 20, logo_ray);
      u8g.drawBox(0 + i, 28, 128, 16);
      u8g.setDefaultBackgroundColor();
      u8g.drawStr(5 + i, 30, F("ArduPLC V2.00") );
      u8g.setDefaultForegroundColor();
      u8g.drawStr(19, 48 - i, F("Ray - 2019"));
    } while ( u8g.nextPage() );
    i += 4;
  }
}

void LedsHmi(void) {
  HMI_IO.digitalWrite (LED_GRE, LOW);
  delay(500);
  HMI_IO.digitalWrite (LED_RED, LOW);
  HMI_IO.digitalWrite (LED_GRE, HIGH);
  delay(500);
  HMI_IO.digitalWrite (LED_ORA, LOW);
  HMI_IO.digitalWrite (LED_RED, HIGH);
  delay(500);
  HMI_IO.digitalWrite (LED_ORA, HIGH);

}

void BotonesHmi() {
  String estado[5];

  for (uint8_t i = 0; i <= 4; i++) {
    if (HMI_IO.digitalRead(BT_UP + i) == 0) {
      estado[i] = "ON";
    } else {
      estado[i] = "OFF";
    }
  }

  Serial.print(F("Button UP: "));
  Serial.println(estado[0]);
  Serial.print(F("Button DOWN: "));
  Serial.println(estado[1]);
  Serial.print(F("Button LEFT: "));
  Serial.println(estado[2]);
  Serial.print(F("Button RIGHT: "));
  Serial.println(estado[3]);
  Serial.print(F("Button INTRO: "));
  Serial.println(estado[4]);

}

void LedBuzzer() {
  digitalWrite(LEDBUZZ, HIGH);
  delay(500);
  digitalWrite(LEDBUZZ, LOW);
  delay(500);
  digitalWrite(LEDBUZZ, HIGH);
  delay(500);
  digitalWrite(LEDBUZZ, LOW);
}

void ErrorI2c() {
  byte error;
  Wire.beginTransmission(0x20);
  error = Wire.endTransmission();
  if (error == 0) {
    //FUNICONA
    //Serial.println("HMI CONECTADO");
  } else {
    Serial.println(F("HMI NO CONECTADO, REVISAR CONEXION"));
  }
}

void EscribirOk() {
  Serial.println(F("OK"));
}

void ReleOn(char rele) {
  byte relay;
  relay = byte(rele) - 0x30;
  if (relay == 1) digitalWrite(RELE1, HIGH);
  if (relay == 2) digitalWrite(RELE2, HIGH);
  if (relay == 3) digitalWrite(RELE3, HIGH);
  if (relay == 4) digitalWrite(RELE4, HIGH);
}

void ReleOff(char rele) {
  byte relay;
  relay = byte(rele) - 0x30;
  if (relay == 1) digitalWrite(RELE1, LOW);
  if (relay == 2) digitalWrite(RELE2, LOW);
  if (relay == 3) digitalWrite(RELE3, LOW);
  if (relay == 4) digitalWrite(RELE4, LOW);
}

void AnalogR() {
  int an1, an2, an3, an4, an5, an6;
  float v1, v2, v3, v4, pot1, pot2;
  an1 = analogRead(AIN1);
  an2 = analogRead(AIN2);
  an3 = analogRead(AIN3);
  an4 = analogRead(AIN4);
  an5 = analogRead(POT1);
  an6 = analogRead(POT2);

  v1 = ((float)an1 * 10.0) / 1023.0;
  v2 = ((float)an2 * 10.0) / 1023.0;
  v3 = ((float)an3 * 10.0) / 1023.0;
  v4 = ((float)an4 * 10.0) / 1023.0;

  pot1 = ((float)an5 * 100.0) / 1023.0;
  pot2 = ((float)an6 * 100.0) / 1023.0;

  Serial.print(F("AN1: "));
  Serial.print(v1, 3);
  Serial.println(F(" v"));
  Serial.print(F("AN2: "));
  Serial.print(v2, 3);
  Serial.println(F(" v"));
  Serial.print(F("AN3: "));
  Serial.print(v3, 3);
  Serial.println(F(" v"));
  Serial.print(F("AN4: "));
  Serial.print(v4, 3);
  Serial.println(F(" v"));

  Serial.print(F("POT1: "));
  Serial.print(pot1, 0);
  Serial.println(F(" %"));
  Serial.print(F("POT2: "));
  Serial.print(pot2, 0);
  Serial.println(F(" %"));
}


void DigitalR() {
  Serial.print(F("DIN1 : "));
  Serial.println(!digitalRead(DIN1));

  Serial.print(F("DIN2 : "));
  Serial.println(!digitalRead(DIN2));

  Serial.print(F("DIN3 : "));
  Serial.println(!digitalRead(DIN3));

  Serial.print(F("DIN4 : "));
  Serial.println(!digitalRead(DIN4));
}

String LeeFecha(void) {
  return (rtc.formatDate(RTCC_DATE_WORLD));
}

String LeeHora(void) {
  return (rtc.formatTime());
}

void EscribeFecha(String mDate) {
  //day, weekday, month, century(1=1900, 0=2000), year(0-99)
  int dia = mDate.substring(0, 2).toInt();
  int mes = mDate.substring(2, 4).toInt();
  int ano = mDate.substring(4, 6).toInt();
  rtc.setDate(dia, 1, mes, 0, ano);
}

void EscribeHora(String mTime) {
  rtc.setTime(mTime.substring(0, 2).toInt(), mTime.substring(2, 4).toInt(), mTime.substring(4, 6).toInt());
}


void GestionaOled(void) {
  int h, w, temp, n;
  char sTemp[30];
  static unsigned long timerOled = millis();

  // Gestiona la pantalla cada 100mS
  if (millis() - timerOled >= 100) {
    timerOled = millis();
    
  u8g.firstPage();
  do {

    // Configuración de formato de  fuente para representanción de las entradas analógicas
    u8g.setFont(u8g_font_5x8r);
    u8g.setFontRefHeightText();
    u8g.setFontPosTop();
    h = u8g.getFontAscent() - u8g.getFontDescent();
    w = u8g.getWidth();

    //Representación de los bordes de la interfaz
    u8g.drawFrame(0, 0, 128, 57);
    u8g.drawLine(44, 0, 44, 57);
    u8g.drawLine (0, 4 * h + 2, 44, 4 * h + 2);

    // Visualizacion de entradas analógicas
    sprintf(sTemp, "AI1:%4d", analogRead(AIN1));
    u8g.drawStr(2, 0 * h + 1 , sTemp);

    sprintf(sTemp, "AI2:%4d", analogRead(AIN2));
    u8g.drawStr(2, 1 * h + 1 , sTemp);

    sprintf(sTemp, "AI3:%4d", analogRead(AIN3));
    u8g.drawStr(2, 2 * h + 1, sTemp);

    sprintf(sTemp, "AI4:%4d", analogRead(AIN4));
    u8g.drawStr(2, 3 * h + 1, sTemp);

    // Visualización de fecha y hora
    sprintf(sTemp, "%s - %s", rtc.formatDate(RTCC_DATE_WORLD), rtc.formatTime());
    u8g.drawStr(12, 8 * h + 1, sTemp);

    // Configuración de formato de  fuente para representanción de los botones del expansor I2C.
    u8g.setFont(u8g_font_6x10r);
    u8g.setFontRefHeightText();
    u8g.setFontPosTop();
    u8g.setFontPosCenter();

    // Leemos los botones del expansor I2C y representamos por pantalla qué botón se pulsa.
    if (HMI_IO.digitalRead(BT_UP) == 0) {
      uint8_t d;
      d = (44 - u8g.getStrWidth(F("UP"))) / 2 + 2;
      u8g.drawStr( d, 57 - ((57 - (4 * h + 2)) / 2), F("UP") );
    }
    else if (HMI_IO.digitalRead(BT_DOWN) == 0) {
      uint8_t d;
      d = (44 - u8g.getStrWidth(F("DOWN"))) / 2 + 2;
      u8g.drawStr( d, 57 - ((57 - (4 * h + 2)) / 2), F("DOWN") );
    }
    else if (HMI_IO.digitalRead(BT_INTRO) == 0) {
      uint8_t d;
      d = (44 - u8g.getStrWidth(F("INTRO"))) / 2 + 2;
      u8g.drawStr( d, 57 - ((57 - (4 * h + 2)) / 2), F("INTRO") );
    }
    else if (HMI_IO.digitalRead(BT_LEFT) == 0) {
      uint8_t d;
      d = (44 - u8g.getStrWidth(F("LEFT"))) / 2 + 2;
      u8g.drawStr( d, 57 - ((57 - (4 * h + 2)) / 2), F("LEFT") );
    }
    else if (HMI_IO.digitalRead(BT_RIGHT) == 0) {
      uint8_t d;
      d = (44 - u8g.getStrWidth(F("RIGHT"))) / 2 + 2;
      u8g.drawStr( d, 57 - ((57 - (4 * h + 2)) / 2), F("RIGHT") );
    }

    // Configuración de formato de fuente para representanción de los potenciómetros y relés.
    u8g.setFont(u8g_font_6x10r);
    u8g.setFontRefHeightText();
    u8g.setFontPosTop();
    h = u8g.getFontAscent() - u8g.getFontDescent();
    w = u8g.getWidth();

    // Visualiza potenciometros...
    n = (int)((analogRead(POT1) * 100L) / 1023L);
    sprintf(sTemp, "POT1:%3d%%", n);
    u8g.drawStr(50, 2 * h + 1 , sTemp);

    n = (int)((analogRead(POT2) * 100L) / 1023L);
    sprintf(sTemp, "POT2:%3d%%", n);
    u8g.drawStr(50, 3 * h + 1 , sTemp);


    // Visualiza entradas digitales...
    u8g.drawStr(50, 2 , F("DIN:"));
    u8g.drawStr(50, 1 * h + 1 ,     F("       4321"));
    for (n = 0; n <= 3; n++) {
      if (!digitalRead(DIN4 - n))
        u8g.drawBox(51 + (6 * (7 + n)), 4, 2, 4);
      u8g.drawFrame(49 + (6 * (7 + n)), 2, 6, 8);
    }

    // Visualiza salidas rele...
    u8g.drawStr(50, 5 * h - 1 , F("RELAY:"));
    u8g.drawStr(50, 4 * h + 1, F("       1234"));
    for (n = 0; n <= 3; n++) {
      if (digitalRead(RELE1 + n))
        u8g.drawBox(51 + (6 * (7 + n)), 48, 2, 4);
      u8g.drawFrame(49 + (6 * (7 + n)), 46, 6, 8);
    }

  } while (u8g.nextPage());
  }
}

/*void test_sd(void){
   // we'll use the initialization code from the utility libraries
  // since we're just testing if the card is working!
  if(digitalRead(INSERT_uSD) == 1){
    Serial.println("uSD no preset.");
    return;
  }

  if (!card.init(SPI_HALF_SPEED, CS_uSD)) {
    Serial.println(F("initialization failed. Things to check:"));
    Serial.println(F("* is a card inserted?"));
    Serial.println(F("* is your wiring correct?"));
    Serial.println(F("* did you change the chipSelect pin to match your shield or module?"));
    return;
  } else {
    Serial.println(F("Wiring is correct and a card is present."));
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
  }*/
