/**************************************************************************
 @FILE: 		ArduPLCnano.cpp
 @AUTHOR:      	Raimundo Alfonso
 @COMPANY:		Ray Ingeniería Electronica, S.L.
 @DESCRIPTION: 	Librería para gestión de ArduPLC NANO
  
 @LICENCE DETAILS:
	Esta librería está basada en software libre. Tu puedes redistribuir
	y/o modificar esta librería bajo los términos de licencia GNU.

	Esta biblioteca se distribuye con la esperanza de que sea útil,
	pero SIN NINGUNA GARANTÍA, incluso sin la garantía implícita de
	COMERCIALIZACIÓN O PARA UN PROPÓSITO PARTICULAR.
	Consulte los términos de licencia GNU para más detalles:
                                                                       
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

  
#ifndef ArduPLCnano_h
#define ArduPLCnano_h

#include <stdint.h>
#include "Arduino.h"

#define ON		1
#define OFF		0
#define RELAY1  4
#define RELAY2  5
#define RELAY3  6
#define RELAY4  7
#define DIN1    0
#define DIN2    1
#define DIN3    9
#define DIN4    10
#define BUTTON_UP    11
#define BUTTON_DOWN  12
#define BUTTON_LEFT  13
#define BUTTON_RIGHT A0


class ArduPLCnano{
  private:

  public:
	ArduPLCnano();
	void relayWrite(uint8_t relay, bool status);	// Escribe estado de un relé
	bool relayRead(uint8_t relay);					// Lee estado de un relé
	void relayToggle(uint8_t relay);				// Alterna el estado de un relé
	bool digitalRead(uint8_t din);					// Lee el estado de una entrada digital o boton
};


#endif  // #ifndef ArduPLCnano_h
