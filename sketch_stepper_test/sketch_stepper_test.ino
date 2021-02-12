
#include <AccelStepper.h>

#define ROTATION_STEPS            771               //Defines the number of steps needed to move to max position
#define MAX_UV_INDEX              11

#define STEPPER_PIN_A1           D5
#define STEPPER_PIN_A2           D6
#define STEPPER_PIN_B1           D7
#define STEPPER_PIN_B2           D8
#define STEPPER_TYPE             4

AccelStepper stepper = AccelStepper(STEPPER_TYPE, STEPPER_PIN_A1, STEPPER_PIN_A2, STEPPER_PIN_B1, STEPPER_PIN_B2);

//Run once setup
void setup() {
  Serial.begin(115200);
  stepper.setMaxSpeed(300);
  stepper.setAcceleration(300.0);
}

void loop() {
  Serial.println("Moving to max");
  stepper.runToNewPosition(ROTATION_STEPS);
  delay(5000);
  Serial.println("Moving to 0");
  stepper.runToNewPosition(0);
}
