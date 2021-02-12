int CLK = 2;//CLK->D2
int DT = 3;//DT->D3
int SW = 4;//SW->D4
// Interrupt 0 on pin 2
const int interrupt0 = 0;
//Define the count
int count = 0;
//CLK initial value
int lastCLK = 0;

void setup() {
  pinMode(SW, INPUT);
  digitalWrite(SW, HIGH);
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  //Set the interrupt 0 handler, trigger level change
  attachInterrupt(interrupt0, ClockChanged, CHANGE);
  Serial.begin(9600);
}

void loop()
{
  //Read the button press and the count value to 0 when the counter reset
  if (!digitalRead(SW) && count != 0) {
    count = 0;
    Serial.print("count:");
    Serial.println(count);
  }

}

//The interrupt handlers
void ClockChanged() {
  //Read the CLK pin level
  int clkValue = digitalRead(CLK);
  //Read the DT pin level
  int dtValue = digitalRead(DT);
  if (lastCLK != clkValue) {
    lastCLK = clkValue;
    //CLK and inconsistent DT + 1, otherwise - 1
    count += (clkValue != dtValue ? 1 : -1);
    Serial.print("count:");
    Serial.println(count);
  }
}
