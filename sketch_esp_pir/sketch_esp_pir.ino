#include <SimpleTimer.h>    //https://github.com/marcelloromani/Arduino-SimpleTimer/tree/master/SimpleTimer
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>    //if you get an error here you need to install the ESP8266 board manager 
#include <PubSubClient.h>
#include <ArduinoOTA.h>     //https://github.com/esp8266/Arduino/tree/master/libraries/ArduinoOTA

#include <DHT.h>;

#include "arduino_secrets"

const wifi_ssid[] = SECRET_WIFI_SSID;
const wifi_pass[] = SECRET_WIFI_PASS;
const mqtt_server[] = SECRET_MQTT_SERVER;

#define MQTT_PORT            1883
#define DEVICE_NAME     "UpstairsHallPIRandTempSensor"         // Used to define MQTT topics, MQTT Client ID, and ArduinoOTA
#define MQTT_STATE_TOPIC "home/upstairs-hall/pir01/state"
#define MQTT_AVAILABILITY_TOPIC "home/upstairs-hall/pir01/online"
#define MQTT_IP_TOPIC "home/upstairs-hall/pir01/ip"

#define DHTPIN D2     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino
int pirPin = D1;               // choose the input pin (for PIR sensor)

#define DOOR_REED_PIN  D5 


WiFiClient espClient;
PubSubClient client(espClient);
SimpleTimer timer;


char msg[200];
char str_temp[6];
char str_hum[6];

void setup_wifi() {
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  WiFi.begin(wifi_ssid, wifi_pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect()  {
  int retries = 0;
  while (!client.connected()) {
    if(retries < 150) {
      Serial.print("Attempting MQTT connection...");
      if (client.connect(DEVICE_NAME, MQTT_AVAILABILITY_TOPIC, 1, true, "false")) {
        Serial.println("connected");
        client.publish(MQTT_AVAILABILITY_TOPIC, "true", true);
        //sprintf(msg, "%s", WiFi.localIP().toString());
        client.publish(MQTT_IP_TOPIC, WiFi.localIP().toString().c_str(), true);
      } 
      else {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        retries++;
        // Wait 5 seconds before retrying
        delay(5000);
      }
    }
    if(retries > 149) {
    ESP.restart();
    }
  }
}

int pirState = LOW;             // we start, assuming no motion detected
int lastPIRState = -1;                    // variable for reading the pin status
float hum;  //Stores humidity value
int intHum;
int lastHum = -1;
float temp; //Stores temperature value
int intTemp;
char strTemp[6];
int lastTemp = -1;
float hi;
int intHI;

int doorState = LOW;
int lastDoorState = LOW;


void checkTemp() {
  hum = dht.readHumidity();
  intHum = int(round(hum));
  temp = dht.readTemperature(true);   // Read in F
  intTemp = int(round(temp * 10));
  hi = dht.computeHeatIndex(temp, hum);
  intHI = int(round(hi));
  /* 4 is mininum width, 2 is precision; float value is copied onto str_temp*/
  dtostrf(temp, 3, 1, strTemp);

  if (intHum != lastHum || intTemp != lastTemp) {
    sendUpdate();
    lastHum = intHum;
    lastTemp = intTemp;
  }
}

void checkPIR() {
  pirState = digitalRead(pirPin);
  if (pirState != lastPIRState) {
    sendUpdate();
    lastPIRState = pirState;
  }
}

void checkDoor() {
  doorState = digitalRead(DOOR_REED_PIN);
  if (doorState != lastDoorState) {
    sendUpdate();
    lastDoorState = doorState;
  }
}

void sendUpdate() {
  sprintf(msg, "{\"motion\": %d, \"temp\": %s, \"hum\": %d, \"hi\": %d, \"door\": %d}", pirState, strTemp, intHum, intHI, doorState);
  Serial.println(msg);
  client.publish(MQTT_STATE_TOPIC, msg);
  client.publish(MQTT_AVAILABILITY_TOPIC, "true", true);
}

void setup() {
  Serial.begin(115200);
  delay(5000);
  
  pinMode(pirPin, INPUT);     // declare sensor as input
  pinMode(DOOR_REED_PIN, INPUT_PULLUP);
  dht.begin();

  WiFi.mode(WIFI_STA);
  setup_wifi();
  client.setServer(mqtt_server, MQTT_PORT);
  ArduinoOTA.setHostname(DEVICE_NAME);
  ArduinoOTA.begin(); 
  delay(10);
  
  timer.setInterval(2000, checkTemp);
  timer.setInterval(2000, checkPIR);
  timer.setInterval(2000, checkDoor);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    setup_wifi();
  }
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  ArduinoOTA.handle();
  timer.run();
}
