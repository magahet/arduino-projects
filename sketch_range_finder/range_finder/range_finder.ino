#include <NewPing.h>
#include "Volume3.h"
#include "pitches.h"

uint16_t frequency = NOTE_C4;

#define TRIGGER_PIN  12  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     11  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 255 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

// For rotary switch
#define CLK 2
#define DT 3
#define SW 4

int redpin = 7; //select the pin for the red LED
int greenpin = 6;// select the pin for the green LED
int bluepin = 5; // select the pin for the  blue LED

const int interrupt0 = 0;
int lastCLK = 0;//CLK initial value

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

const int buzzer = 9; //buzzer to arduino pin 9
int distance = MAX_DISTANCE;
unsigned long current, lastBeep, lastPing;  //some global variables available anywhere in the program

#define MAX 1023
#define MIN 0
#define ROTATE_VALUE 16
volatile int VOLUME = 128;
bool ON = false;

void setup(){
  pinMode(redpin, OUTPUT); pinMode(bluepin, OUTPUT); pinMode(greenpin, OUTPUT);
  
  //Serial.begin(9600); // Open serial monitor at 115200 baud to see ping results.
  pinMode(buzzer, OUTPUT); // Set buzzer - pin 9 as an output
  lastBeep = millis();  //initial start time
  lastPing = millis();  //initial start time

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(SW, INPUT);
  digitalWrite(SW, HIGH);
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  
  attachInterrupt(interrupt0, ClockChanged, CHANGE);//Set the interrupt 0 handler, trigger level change
}

void led(int r, int g, int b) {
  analogWrite(redpin, r);
  analogWrite(greenpin, g);
  analogWrite(bluepin, b);
}

void loop(){
  if (!digitalRead(SW)) {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    ON = !ON;
    delay(500);
  }

  if (!ON) {
    delay(100);
    return;
  }
  
  current = millis();
  
  if (current - lastBeep >= distance * 7) {
    vol.tone(9, frequency, VOLUME);
    delay(100);
    vol.noTone();
    lastBeep = current;
    led(255 - distance, distance, 0 );
    //Serial.println(distance);
  }
  
  if (current - lastPing >= 50) {
    distance = sonar.ping_cm();
    if (distance == 0) {
      distance = MAX_DISTANCE;
    }
    lastPing = current;
  }

}

void ButtonPress() {
  ON = !ON;
}

void ClockChanged() {
  int clkValue = digitalRead(CLK);//Read the CLK pin level
  int dtValue = digitalRead(DT);//Read the DT pin level
  if (lastCLK != clkValue) {
    lastCLK = clkValue;
    int newVolume = VOLUME + (clkValue != dtValue ? -ROTATE_VALUE : ROTATE_VALUE);
    VOLUME = max(min(newVolume, MAX), MIN);
  }
}
