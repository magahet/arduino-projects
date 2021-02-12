#include <SimpleTimer.h>    //https://github.com/marcelloromani/Arduino-SimpleTimer/tree/master/SimpleTimer
#include <WiFiEsp.h>
#include <WiFiEspClient.h>
#include <WiFiEspUdp.h>
#include "SoftwareSerial.h"
#include <PubSubClient.h>
#include <Adafruit_SleepyDog.h>

#include "arduino_secrets"

const wifi_ssid[] = SECRET_WIFI_SSID;
const wifi_pass[] = SECRET_WIFI_PASS;
const mqtt_server[] = SECRET_MQTT_SERVER;


#define MQTT_PORT            1883
#define DEVICE_NAME     "PoolController"         // Used to define MQTT topics, MQTT Client ID, and ArduinoOTA

#define MQTT_AVAILABILITY_TOPIC "home/pool/controller/online"
//#define MQTT_IP_TOPIC "home/pool/controller/ip"
#define MQTT_POOL_CONTROLLER_STATE_TOPIC "home/pool/controller/state"


//#define MQTT_PUMP_STATE_TOPIC "home/pool/pump/state"
#define MQTT_PUMP_COMMAND_TOPIC "home/pool/pump/set"

//#define MQTT_HEATER_STATE_TOPIC "home/pool/heater/state"
#define MQTT_HEATER_COMMAND_TOPIC "home/pool/heater/set"

//#define MQTT_TEMP_STATE_TOPIC "home/pool/temp/state"
//#define MQTT_PRESSURE_STATE_TOPIC "home/pool/pressure/state"


#define PRESSURE_PIN      A0
#define TEMP_PIN         A1

#define RX_PIN           8
#define TX_PIN           9

#define PUMP1_PIN        3
#define PUMP2_PIN        4
#define PUMP3_PIN        5
#define PUMP4_PIN        6

#define HEATER_PIN       7

SoftwareSerial SerialESP(RX_PIN, TX_PIN); // RX, TX
WiFiEspClient espClient;
PubSubClient client(espClient);
SimpleTimer timer;
char msg[50];

int pumpState = 0;
int heaterState = 0;
int pumpPins[] = {PUMP1_PIN, PUMP2_PIN, PUMP3_PIN, PUMP4_PIN};

void setup_wifi() {
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);
  WiFi.init(&SerialESP);
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }
  
  Serial.println("Connecting to AP ...");
  // attempt to connect to WiFi network
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
        //client.publish(MQTT_IP_TOPIC, WiFi.localIP().toString().c_str(), true);
        client.subscribe(MQTT_PUMP_COMMAND_TOPIC);
        client.subscribe(MQTT_HEATER_COMMAND_TOPIC);
        client.setCallback(callback);
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
      reset();
    }
  }
}

void reset() {
  Watchdog.enable(1);
  for (;;) {
  }
}

void setup() {
  // initialize serial for debugging
  Serial.begin(115200);
  // initialize serial for ESP module
  SerialESP.begin(115200);
  delay(5000);

  for (int i = 0; i < 4; i++) {
    digitalWrite(pumpPins[i], LOW);
    pinMode(pumpPins[i], OUTPUT);
  }

  digitalWrite(HEATER_PIN, LOW);
  pinMode(HEATER_PIN, OUTPUT);
  
  setup_wifi();
  client.setServer(mqtt_server, MQTT_PORT);
  delay(10);
  
  timer.setInterval(5000, checkState);
}



int checkTemp() {
  int Vo;
  float R1 = 10000;
  float logR2, R2, T;
  float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
  
  Vo = analogRead(TEMP_PIN);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  T = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
  T = T - 273.15;
  T = (T * 9.0)/ 5.0 + 32.0;
  return int(round(T));
}


int checkPresure() {
  int Vo, psi;
  
  Vo = analogRead(PRESSURE_PIN);
  psi = map(Vo, 102, 921, 0, 80);
  return psi;
}

void checkState() {
  int temp = checkTemp();  
  int psi = checkPresure();
  
  sprintf(msg, "{\"pump\": %d, \"temp\": %d, \"pressure\": %d, \"heater\": %d}", pumpState, temp, psi, heaterState);
  Serial.println(msg);
  client.publish(MQTT_POOL_CONTROLLER_STATE_TOPIC, msg);

}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String mqtt_data = "";
  for (int i = 0; i < length; i++) {
    mqtt_data += (char)payload[i];
  }
  Serial.println(mqtt_data);

  if (topic == MQTT_PUMP_COMMAND_TOPIC) {
    int newPumpState = mqtt_data.toInt();
    if (newPumpState > 0) {
      digitalWrite(pumpPins[pumpState], LOW);
      digitalWrite(pumpPins[newPumpState], HIGH);
    }
    pumpState = newPumpState;
  }
  
  if (topic == MQTT_HEATER_COMMAND_TOPIC) {
    if (mqtt_data == "0") {
      heaterState = LOW;
      digitalWrite(HEATER_PIN, LOW);
    } else if (mqtt_data == "1") {
      heaterState = HIGH;
      digitalWrite(HEATER_PIN, HIGH);
    }
  }

}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    setup_wifi();
  }
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  timer.run();
}
