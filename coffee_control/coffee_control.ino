#include <SimpleTimer.h>    //https://github.com/marcelloromani/Arduino-SimpleTimer/tree/master/SimpleTimer
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>    //if you get an error here you need to install the ESP8266 board manager 
#include <PubSubClient.h>
#include <ArduinoOTA.h>     //https://github.com/esp8266/Arduino/tree/master/libraries/ArduinoOTA

#include "arduino_secrets"

const wifi_ssid[] = SECRET_SSID;
const wifi_pass[] = SECRET_WIFI_PASS;
const mqtt_server[] = SECRET_MQTT_SERVER;

#define MQTT_PORT            1883
#define DEVICE_NAME     "CoffeeMakerController"         // Used to define MQTT topics, MQTT Client ID, and ArduinoOTA
#define MQTT_STATE_TOPIC "home/kitchen/coffee_maker/state"
#define MQTT_COMMAND_TOPIC "home/kitchen/coffee_maker/set"
#define MQTT_AVAILABILITY_TOPIC "home/kitchen/coffee_maker/online"
#define MQTT_IP_TOPIC "home/kitchen/coffee_maker/ip"

#define LED_PIN         D2
#define BUTTON_PIN      D1

WiFiClient espClient;
PubSubClient client(espClient);
SimpleTimer timer;
char msg[50];

int lastState = 2;

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
        client.subscribe(MQTT_COMMAND_TOPIC);
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
    ESP.restart();
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(5000);
  digitalWrite(BUTTON_PIN, LOW);
  pinMode(BUTTON_PIN, OUTPUT);
  
  WiFi.mode(WIFI_STA);
  setup_wifi();
  client.setServer(mqtt_server, MQTT_PORT);
  ArduinoOTA.setHostname(DEVICE_NAME);
  ArduinoOTA.begin(); 
  delay(10);
  
  timer.setInterval(2000, checkState);
}

void checkState() {
  int state = digitalRead(LED_PIN);

  // DEBUG
  int v = analogRead(LED_PIN);
  sprintf(msg, "%d", v);
  client.publish("debug/coffe_maker/led_v", msg);

  
  if (state != lastState) {
    snprintf(msg, 50, "%d", state);
    Serial.println(msg);
    client.publish(MQTT_STATE_TOPIC, msg);
    lastState = state;
  }
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
  if (mqtt_data == "1") {
    Serial.println("Pushing Button");
    digitalWrite(BUTTON_PIN, HIGH);
    delay(200);
    digitalWrite(BUTTON_PIN, LOW);
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
  ArduinoOTA.handle();
  timer.run();
}
