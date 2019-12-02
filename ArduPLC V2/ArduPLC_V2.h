/*
  ArduPLC_V2.h - ArduPLC V2 library
  Copyright (c) 2014 Raimundo Alfonso
  Ray Ingeniería Electrónica, S.L.
  
  Esta librería está basada en software libre. Tu puedes redistribuir
  y/o modificar esta librería bajo los terminos de licencia GNU.

  Esta biblioteca se distribuye con la esperanza de que sea útil,
  pero SIN NINGUNA GARANTÍA, incluso sin la garantía implícita de
  COMERCIALIZACIÓN O PARA UN PROPÓSITO PARTICULAR.
  Consulte los terminos de licencia GNU para más detalles.
*/

#include <Wire.h>
#include <Rtc_Pcf8563.h>	// https://playground.arduino.cc/Main/RTC-PCF8563/

#define ArduPLC 1



#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#define AIN1	0
#define AIN2	1
#define AIN3	2
#define AIN4	3
#define RELE1   7
#define RELE2   8
#define RELE3   9
#define RELE4   10
#define DIN1    3
#define DIN2    4
#define DIN3    5
#define DIN4    6
 
#define ADDR_RTC   0x51  // Direccion I2C del RTC
 
 
 class ArduPLC_V2{
	public:
	
	ArduPLC_V2();
	void releOn(byte);
	void releOff(byte);
	boolean leeDIN(byte digIn);
	int leeAIN(byte anIn);	
	int leePOT(byte pot);
	String leeFecha(void);
	String leeHora(void);
	void escribeFecha(String);
	void escribeHora(String);
	

	
	private:
	byte reles[4];
	byte din[4];
	Rtc_Pcf8563 rtc;	
	
 
 };
