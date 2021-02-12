int inputPin = 0;
int Vo;
int psi;

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  Vo = analogRead(inputPin);
  psi = map(Vo, 102, 921, 0, 80);

  Serial.print("Presure: "); 
  Serial.print(psi);
  Serial.println(" psi"); 

  delay(500);
}
