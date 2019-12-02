/*
  ArduPLCm.h - ArduPLCm library
  Copyright (c) 2012 Raimundo Alfonso
  Ray Ingeniería Electrónica, S.L.
  
  Esta librería está basada en software libre. Tu puedes redistribuir
  y/o modificar esta librería bajo los terminos de licencia GNU.

  Esta biblioteca se distribuye con la esperanza de que sea útil,
  pero SIN NINGUNA GARANTÍA, incluso sin la garantía implícita de
  COMERCIALIZACIÓN O PARA UN PROPÓSITO PARTICULAR.
  Consulte los terminos de licencia GNU para más detalles.
*/

#ifndef ArduPLCm_h
#define ArduPLCm_h

#include "Arduino.h"

#define DIN1	2
#define DIN2	3
#define DIN3	18
#define DIN4	19
#define DIPSW1	4
#define DIPSW2	5
#define DIPSW3	6
#define DIPSW4	7
#define DIPSW5	8
#define DIPSW6	9
#define RELE1	10
#define RELE2	11
#define RELE3	12
#define RELE4	13
#define AIN1	0
#define AIN2	1
#define AIN3	2
#define AIN4	3
 
 
 class ArduPLCm{
	public:
	
	ArduPLCm();
	void releOn(byte);
	void releOff(byte);
	byte leeDIPSW(void);
	boolean leeDIN(byte digIn);
	int leeAIN(byte anIn);	
	float leeVoltiosAIN(byte anIn);
	
	private:
 
 
 
 };





#endif

