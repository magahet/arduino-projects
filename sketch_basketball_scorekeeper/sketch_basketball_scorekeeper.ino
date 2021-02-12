#include <NewPing.h>

#define TRIGGER_PIN  12  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     11  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define TRIGGER_DISTANCE 25 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
#define INTERVAL 20

const int interrupt0 = 0;
int lastCLK = 0;//CLK initial value

NewPing sonar(TRIGGER_PIN, ECHO_PIN, 170); // NewPing setup of pins and maximum distance.

int distance, score;
unsigned long current, lastBeep, lastPing;  //some global variables available anywhere in the program

void setup(){
  Serial.begin(9600); // Open serial monitor at 115200 baud to see ping results.
}

void loop(){
    
  current = millis();
  
  
  if (current - lastPing >= INTERVAL) {
    distance = sonar.ping_cm();
    if (distance > 5 && distance < TRIGGER_DISTANCE) {
      score++;
      Serial.println(score);
      delay(500);
    }
    lastPing = current;
  }

}
