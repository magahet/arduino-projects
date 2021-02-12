#include "Volume3.h" // Include the Volume library
#define speakerPin 9
String command;

void setup() {
  // put your setup code here, to run once:
  //tone(3, 400, 500);
  Serial.begin(9600); // opens serial port, sets data rate to 9600 bps

}

uint16_t volume = 128;
uint16_t frequency = 220;

void loop() {

  
  if(Serial.available()){
        command = Serial.readStringUntil('\n');
         
        if(command.equals("p")){
          play(5);
        }
        else if(command.equals("send")){
            //send_message();
        }
    }
}

void play(int top) {
  for (int i = 1; i <= top; i++) {
    vol.tone(speakerPin, frequency * i, volume);
    delay(100);
    //vol.noTone();
    //delay(100);
  }
  vol.noTone();
}

