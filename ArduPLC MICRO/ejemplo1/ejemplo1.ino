 // Mapa de pines ArduPLC MICRO
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

#define CAL_5V  5.000   // Para mayor precisión en la medida de entradas analógicas, mide el voltaje en la borna auxiliar de 5V y copialo aquí.


void setup() {
  Serial.begin(9600);
  
  // Configuración de Relés
  pinMode(RELE1, OUTPUT);
  pinMode(RELE2, OUTPUT);
  pinMode(RELE3, OUTPUT);
  pinMode(RELE4, OUTPUT);

  // Configuración de las entradas digitales
  pinMode(DIN1, INPUT);
  pinMode(DIN2, INPUT);
  pinMode(DIN3, OUTPUT);
  pinMode(DIN4, INPUT);
  pinMode(DIN5, INPUT);
  pinMode(DIN6, INPUT);
}

void loop() {
  int an1;
  int v1;
  
  // Activa RELE4 en función de la entrada digital1
  bool boton = digitalRead(DIN1);
  (boton == 1) ? digitalWrite(RELE4, HIGH) : digitalWrite(RELE4,LOW);
  
  // Salida PWM en DIN3 en función de la entrada analógica 1
  an1 = analogRead(AIN1);
  v1 = ((float)an1 *255)/511;
  analogWrite(DIN3,v1);
  Serial.println(v1);
  
}
