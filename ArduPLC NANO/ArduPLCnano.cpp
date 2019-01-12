/**************************************************************************
 @FILE: 		ArduPLCnano.cpp
 @AUTHOR:      	Raimundo Alfonso
 @COMPANY:		Ray Ingenieria Electronica, S.L.
 @DESCRIPTION: 	Libreria para gestión de ArduPLC NANO
  
 @LICENCE DETAILS:
	Esta librería está basada en software libre. Tu puedes redistribuir
	y/o modificar esta librería bajo los terminos de licencia GNU.

	Esta biblioteca se distribuye con la esperanza de que sea útil,
	pero SIN NINGUNA GARANTÍA, incluso sin la garantía implícita de
	COMERCIALIZACIÓN O PARA UN PROPÓSITO PARTICULAR.
	Consulte los terminos de licencia GNU para más detalles:
                                                                       
	http://www.gnu.org/licenses/gpl-3.0.txt
	
	This library is based on free software. You can redistribute
	and/or modify this library under the terms of the GNU license.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY, even without the implied warranty of
	MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
	See the GNU license terms for more details:		
	
	http://www.gnu.org/licenses/gpl-3.0.txt

 @VERSIONS:
	01-12-2016 - v1.00 :
  
**************************************************************************/
  
  
/**************************************************************************
 * Includes
 *************************************************************************/  
#include "ArduPLCnano.h"
#include <stddef.h>
#include <stdint.h>
  
  
/**************************************************************************
 * Constructors
 *************************************************************************/
 ArduPLCnano::ArduPLCnano(void){
	pinMode(RELAY1, OUTPUT);
	pinMode(RELAY2, OUTPUT);
	pinMode(RELAY3, OUTPUT);
	pinMode(RELAY4, OUTPUT);
	pinMode(DIN1, INPUT);
	pinMode(DIN2, INPUT);
	pinMode(DIN3, INPUT);
	pinMode(DIN4, INPUT);
	pinMode(BUTTON_UP, INPUT);
	pinMode(BUTTON_DOWN, INPUT);
	pinMode(BUTTON_LEFT, INPUT);
}
 
 /**************************************************************************
 * User functions
 **************************************************************************/
 void ArduPLCnano::relayWrite(uint8_t relay, bool status){
	::digitalWrite(relay,status);
}

 bool ArduPLCnano::relayRead(uint8_t relay){
	return(::digitalRead(relay));
}

 void ArduPLCnano::relayToggle(uint8_t relay){
	::digitalWrite(relay,!::digitalRead(relay));
}

bool ArduPLCnano::digitalRead(uint8_t din){
	if(din == BUTTON_RIGHT){
		if(::analogRead(BUTTON_RIGHT)<512)
			return(ON);
		else
			return(OFF);
	}else{
		return(!::digitalRead(din));
	}
}






