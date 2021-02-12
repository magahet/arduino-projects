#include <FastLED.h>
#include <math.h>


#define LED_PIN     7
#define MIC_PIN   A0  // Microphone is attached to this analog pin
#define NUM_LEDS    150
#define SAMPLE_WINDOW   10  // Sample window for average level
#define PEAK_HANG 24 //Time of pause before peak dot falls
#define PEAK_FALL 4 //Rate of falling peak dot

#define CLK 2
#define DT 3
#define SW 4

const int interrupt0 = 0;
int lastCLK = 0;//CLK initial value

unsigned int sample;
unsigned int INPUT_FLOOR = 4; //Lower range of analogRead input
unsigned int INPUT_CEILING = 50; //Max range of analogRead input, the lower the value the more sensitive (1023 = max)
 
byte dotCount = 0;  //Frame counter for peak dot
byte dotHangCount = 0; //Frame counter for holding peak dot

bool blink_led = false;


#define MIN_COLOR   96
#define MAX_COLOR   0
#define MIN_VALUE   0
#define MAX_VALUE   1023

int Analog_Pin = A0;
bool HighThreshold = false;



CRGB leds[NUM_LEDS];

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(SW, INPUT);
  digitalWrite(SW, HIGH);
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  attachInterrupt(interrupt0, ClockChanged, CHANGE);//Set the interrupt 0 handler, trigger level change
  //analogReference(EXTERNAL);

  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  Serial.begin(9600);

}

float get_volume() {
  unsigned long startMillis= millis();  // Start of sample window
  float peakToPeak = 0;   // peak-to-peak level
 
  unsigned int signalMax = 0;
  unsigned int signalMin = 1023;
 
 
  // collect data for length of sample window (in mS)
  while (millis() - startMillis < SAMPLE_WINDOW)
  {
    sample = analogRead(MIC_PIN);
    //Serial.println(sample);
    if (sample < 1024)  // toss out spurious readings
    {
      if (sample > signalMax)
      {
        signalMax = sample;  // save just the max levels
      }
      else if (sample < signalMin)
      {
        signalMin = sample;  // save just the min levels
      }
    }
  }
  peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
  return peakToPeak;
  // Serial.println(peakToPeak);
}

float fscale( float originalMin, float originalMax, float newBegin, float
newEnd, float inputValue, float curve){
 
  float OriginalRange = 0;
  float NewRange = 0;
  float zeroRefCurVal = 0;
  float normalizedCurVal = 0;
  float rangedValue = 0;
  boolean invFlag = 0;
 
 
  // condition curve parameter
  // limit range
 
  if (curve > 10) curve = 10;
  if (curve < -10) curve = -10;
 
  curve = (curve * -.1) ; // - invert and scale - this seems more intuitive - postive numbers give more weight to high end on output 
  curve = pow(10, curve); // convert linear scale into lograthimic exponent for other pow function
 
  /*
   Serial.println(curve * 100, DEC);   // multply by 100 to preserve resolution  
   Serial.println(); 
   */
 
  // Check for out of range inputValues
  if (inputValue < originalMin) {
    inputValue = originalMin;
  }
  if (inputValue > originalMax) {
    inputValue = originalMax;
  }
 
  // Zero Refference the values
  OriginalRange = originalMax - originalMin;
 
  if (newEnd > newBegin){ 
    NewRange = newEnd - newBegin;
  }
  else
  {
    NewRange = newBegin - newEnd; 
    invFlag = 1;
  }
 
  zeroRefCurVal = inputValue - originalMin;
  normalizedCurVal  =  zeroRefCurVal / OriginalRange;   // normalize to 0 - 1 float
 
  // Check for originalMin > originalMax  - the math for all other cases i.e. negative numbers seems to work out fine 
  if (originalMin > originalMax ) {
    return 0;
  }
 
  if (invFlag == 0){
    rangedValue =  (pow(normalizedCurVal, curve) * NewRange) + newBegin;
 
  }
  else     // invert the ranges
  {   
    rangedValue =  newBegin - (pow(normalizedCurVal, curve) * NewRange); 
  }
 
  return rangedValue;
}

void loop() {
  unsigned int c, y;

  // Toggle threshold setting that changes with rotator
  if (!digitalRead(SW)) {
    HighThreshold = !HighThreshold;
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  }
  
  float peakToPeak = get_volume();
  //Serial.println(peakToPeak);
  c = int(fscale(INPUT_FLOOR, INPUT_CEILING, 0, NUM_LEDS, peakToPeak, 2));
  if (c > 5) {
    //Serial.println(c);
  }
  for (int i = 0; i < c; i++) {
    int color = map(i, 0, c, MIN_COLOR, MAX_COLOR);
    //Serial.println(color);
    leds[i] = CHSV(color, 255, 255);
    FastLED.show();
  }

  if (c > 0) {
    delay(500);
  }
  
  for (int i = c - 1; i >= 0; i--) {
    leds[i] = CRGB::Black;
    FastLED.show();
    //delay(30);
  }

  if (blink_led) {
    for (int i = 0; i < 8; i++) {
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
      delay(100);
    }
    blink_led = false;
  }
}


//The interrupt handlers
void ClockChanged() {
  int clkValue = digitalRead(CLK);//Read the CLK pin level
  int dtValue = digitalRead(DT);//Read the DT pin level
  if (lastCLK != clkValue) {
    lastCLK = clkValue;
    int delta = (clkValue != dtValue ? -1 : 1);//CLK and inconsistent DT + 1, otherwise - 1
    if (HighThreshold) {
      if (INPUT_CEILING + delta > 1023 || INPUT_CEILING + delta <= INPUT_FLOOR) {
        blink_led = true;
        return;
      }
      INPUT_CEILING += delta;
    } else {
       if (INPUT_FLOOR + delta < 0 || INPUT_FLOOR + delta >= INPUT_CEILING) {
        blink_led = true;
        return;
      }
      INPUT_FLOOR += delta;
    }
    Serial.print(INPUT_FLOOR);
    Serial.print(" - ");
    Serial.println(INPUT_CEILING);   
  }
}

