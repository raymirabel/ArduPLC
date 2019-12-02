/*
  ArduPLC_V2.cpp - Libreria para la placa ArduPLC V2
  Copyright (c) 2014 Raimundo Alfonso
  Ray Ingeniería Electrónica, S.L.
  
  Esta librería está basada en software libre. Tu puedes redistribuir
  y/o modificar esta librería bajo los terminos de licencia GNU.

  Esta biblioteca se distribuye con la esperanza de que sea útil,
  pero SIN NINGUNA GARANTÍA, incluso sin la garantía implícita de
  COMERCIALIZACIÓN O PARA UN PROPÓSITO PARTICULAR.
  Consulte los terminos de licencia GNU para más detalles.

  09-09-2014 - v1.00 : primera versión
  
  
*/

//#include "WConstants.h"
#include "ArduPLC_V2.h"
#include <Wire.h>
#include <Rtc_Pcf8563.h>	// https://playground.arduino.cc/Main/RTC-PCF8563/

ArduPLC_V2::ArduPLC_V2(void){
	pinMode(DIN1,INPUT);
	pinMode(DIN2,INPUT);	
	pinMode(DIN3,INPUT);
	pinMode(DIN4,INPUT);
	pinMode(RELE1,OUTPUT);
	pinMode(RELE2,OUTPUT);
	pinMode(RELE3,OUTPUT);
	pinMode(RELE4,OUTPUT);
	reles[0] = RELE1;
	reles[1] = RELE2;	
	reles[2] = RELE3;	
	reles[3] = RELE4;	
	din[0] = DIN1;
	din[1] = DIN2;	
	din[2] = DIN3;	
	din[3] = DIN4;
}


void ArduPLC_V2::releOn(byte rele){
	digitalWrite(reles[rele-1],HIGH);
}
 
void ArduPLC_V2::releOff(byte rele){
	if(rele > 4) return;
	if(rele < 1) return;	
	digitalWrite(reles[rele-1],LOW);
} 
 
boolean ArduPLC_V2::leeDIN(byte digIn){
	if(digIn > 4) return false;
	if(digIn < 1) return false;	
	return(digitalRead(din[digIn-1]));
} 

int ArduPLC_V2::leeAIN(byte anIn){
	if(anIn > 4) return -1;
	if(anIn < 1) return -1;	
	return(analogRead(anIn-1));
}

int ArduPLC_V2::leePOT(byte p){
	if(p > 2) return -1;
	if(p < 1) return -1;	
	return(analogRead(p+5));
}


String ArduPLC_V2::leeFecha(void){
	return(rtc.formatDate(RTCC_DATE_WORLD));
}

String ArduPLC_V2::leeHora(void){
	return(rtc.formatTime());
}

void ArduPLC_V2::escribeFecha(String mDate){
	//day, weekday, month, century(1=1900, 0=2000), year(0-99)
	int dia = mDate.substring(0,2).toInt();
	int mes = mDate.substring(2,4).toInt();
	int ano = mDate.substring(4,6).toInt();
	rtc.setDate(dia, 1, mes, 0, ano);
}
	
void ArduPLC_V2::escribeHora(String mTime){
	rtc.setTime(mTime.substring(0,2).toInt(), mTime.substring(2,4).toInt(), mTime.substring(4,6).toInt());
}