int vibr_pin=3;
int LED_Pin=13;

int relayPin1 = 7;                 // IN1 connected to digital pin 7



void setup() {
  pinMode(vibr_pin,INPUT);
  pinMode(LED_Pin,OUTPUT);

  pinMode(relayPin1, OUTPUT);      // sets the digital pin as output
  digitalWrite(relayPin1, HIGH);        // Prevents relays from starting up engaged
}

void loop() {
  int val;
  val=digitalRead(vibr_pin);
  if(val==1)
  {
    digitalWrite(LED_Pin,HIGH);
    digitalWrite(relayPin1,LOW);
    delay(2000);
    digitalWrite(LED_Pin,LOW);
    digitalWrite(relayPin1,HIGH);
    delay(10000);
   }
   else
   {
   digitalWrite(LED_Pin,LOW);
   digitalWrite(relayPin1,HIGH);
   }
}

