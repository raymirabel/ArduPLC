# ArduPLC NANO #
## Mini PLC básico basado en Arduino ##

----------
ArduPLC NANO es un mini PLC (programmable logic controller) basado en arquitectura Arduino. Está pensado y diseñado para usos en entornos industriales y domésticos.  Puedes programar fácilmente ArduPLC NANO en cualquier entorno de desarrollo para Arduino y compatibles. Asegurate que seleccionas como placa destino **Arduino MICRO**.

![ArduX1](https://github.com/raymirabel/ArduPLC_NANO/blob/master/doc/ArduPLC_NANO.png)

Lo más destacado:

- Tamaño compacto.
- Alimentación directa a 230VAC (versión AC) o 24VDC (versión DC).
- Puerto USB MINI.
- 4 relés.
- 4 entradas digitales optoacopladas.
- Conector auxiliar RJ12 para bus de expansión I2C.
- RTC opcional.
- Frontal HMI (pantalla OLED 1.3" y 4 botones) opcional.


Dispones de la librería **ArduPLC_NANO** para facilitar la programación.

## Funciones disponibles: ##

	void relayWrite(uint8_t relay, bool status);	// Escribe estado de un relé
	bool relayRead(uint8_t relay);					// Lee estado de un relé
	void relayToggle(uint8_t relay);				// Alterna el estado de un relé
	bool digitalRead(uint8_t din);					// Lee el estado de una entrada digital


## Palabras clave:     

	#define ON
	#define OFF
	#define RELAY1  
	#define RELAY2  
	#define RELAY3  
	#define RELAY4  
	#define DIN1    
	#define DIN2    
	#define DIN3    
	#define DIN4    
	#define BUTTON_UP
	#define BUTTON_DOWN
	#define BUTTON_LEFT
	#define BUTTON_RIGHT