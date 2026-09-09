//HOLA MUNDO

void setup() 
{
  Serial.begin(115200);
  Serial.print("ESTO ES EL SETUP");
  Serial.println("linea nueva despues.");
}

void loop() 
{
  Serial.println("ESTO ES EL LOOP");
  delay(2000);
}
