int analogPin = 0;
int raw = 0;
int Vin = 5;
float Vout = 0;
float R1 = 1000;
float R2 = 0;
float buffer = 0;

// For rotary switch
#define CLK 2
#define DT 3
#define SW 4
const int interrupt0 = 0;
int lastCLK = 0;//CLK initial value

#define MAX 1.0
#define MIN 0.1
#define ROTATE_VALUE 0.05
volatile float VOLUME = 1.0;


void setup(){
  Serial.begin(9600);
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  attachInterrupt(interrupt0, ClockChanged, CHANGE);//Set the interrupt 0 handler, trigger level change

}

void loop(){
  raw = analogRead(analogPin);
  if(raw){
    buffer = raw * Vin;
    Vout = (buffer)/1024.0;
    buffer = (Vin/Vout) - 1;
    R2= R1 * buffer;
    //Serial.print("Vout: ");
    //Serial.println(Vout);
    //Serial.print("R2: ");
    
    Serial.println(R2 * VOLUME);
    delay(100);
  }
}

void ClockChanged() {
  int clkValue = digitalRead(CLK);//Read the CLK pin level
  int dtValue = digitalRead(DT);//Read the DT pin level
  if (lastCLK != clkValue) {
    lastCLK = clkValue;
    float newVolume = VOLUME + (clkValue != dtValue ? -ROTATE_VALUE : ROTATE_VALUE);
    VOLUME = max(min(newVolume, MAX), MIN);
  }
}
