//SEGUNDO PROGRAMA PRUEBA ESP32
//SEMAFORO CON LED's

#define verde 18
#define rojo 21
#define azul 22

void setup() 
{
  pinMode(verde, OUTPUT);
  pinMode(rojo, OUTPUT);
  pinMode(azul, OUTPUT);

}

void loop()
{
  digitalWrite(verde, HIGH);
  delay(1000);
  digitalWrite(rojo,HIGH);
  delay(1000);
  digitalWrite(azul,HIGH);
  delay(1000);

  digitalWrite(verde, LOW);
  delay(1000);
  digitalWrite(rojo,LOW);
  delay(1000);
  digitalWrite(azul,LOW);
  delay(1000);
}
