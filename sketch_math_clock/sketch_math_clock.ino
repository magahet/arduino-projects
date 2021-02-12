#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <DS3231.h>

DS3231 clock;
RTCDateTime dt;

// Rotary Encoder vars
int CLK = 2;//CLK->D2
int DT = 3;//DT->D3
int SW = 4;//SW->D4
// Interrupt 0 on pin 2
const int interrupt0 = 0;
//Define the count
int count = 0;
//CLK initial value
int lastCLK = 0;

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display


/*********************************************************/
void setup() {
  pinMode(SW, INPUT);
  digitalWrite(SW, HIGH);
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  //Set the interrupt 0 handler, trigger level change
  attachInterrupt(interrupt0, ClockChanged, RISING);


  Serial.begin(9600);
  clock.begin();
  // Set sketch compiling time
  clock.setDateTime(__DATE__, __TIME__);
  //clock.setDateTime(2019, 5, 9, 3, 34, 0);  
  lcd.init();  //initialize the lcd
  lcd.backlight();  //open the backlight 
}
/*********************************************************/
void loop() {
  
  printTime();
  
  if (count != 0) {
    clock.setDateTime(dt.unixtime - 60 * (count / 2));
    count = 0;
  }
  
  delay(500);
}
/************************************************************/

void ClockChanged() {
  // Read the CLK pin level
  int clkValue = digitalRead(CLK);
  // Read the DT pin level
  int dtValue = digitalRead(DT);
  if (lastCLK != clkValue) {
    lastCLK = clkValue;
    // CLK and inconsistent DT + 1, otherwise - 1
    count += (clkValue != dtValue ? 1 : -1);
    //printTime();
    Serial.print("count:");
    Serial.println(count);
  }
}

void printTime() {
  dt = clock.getDateTime();

  lcd.setCursor ( 0, 0 );            // go to the top left corner
  lcd.print(mkProb((int) dt.month)); lcd.print("/"); lcd.print(dt.day); lcd.print("/"); lcd.print(dt.year);
  
  lcd.setCursor ( 0, 2 );            // go to the top left corner
  lcd.print(dt.hour); lcd.print(":"); lcd.print(dt.minute);

//  Serial.print(dt.year);
//  Serial.print("-");
//  Serial.print(dt.month); Serial.print("-");
//  Serial.print(dt.day); Serial.print(" ");
//  Serial.print(dt.hour); Serial.print(":");
//  Serial.print(dt.minute); Serial.print(":");
//  Serial.print(dt.second); Serial.println("");
//  Serial.print(dt.unixtime); Serial.println("");

}

long randNum;

String mkProb(int i) {
  return "hello";
  randNum = random(100);
  if (randNum > i) {
    if (randNum % i == 0) {
      //return String(String(randNum) + " / " + String(randNum / i));
    }
  }
}


