
#include <SimpleTimer.h>    //https://github.com/marcelloromani/Arduino-SimpleTimer/tree/master/SimpleTimer
#include <ESP8266WiFi.h>    //if you get an error here you need to install the ESP8266 board manager 
#include <ESP8266mDNS.h>    //if you get an error here you need to install the ESP8266 board manager 
#include <PubSubClient.h>   //https://github.com/knolleary/pubsubclient
#include <ArduinoOTA.h>     //https://github.com/esp8266/Arduino/tree/master/libraries/ArduinoOTA
#include <AccelStepper.h>

/*****************  START USER CONFIG SECTION *********************************/
/*****************  START USER CONFIG SECTION *********************************/
/*****************  START USER CONFIG SECTION *********************************/
/*****************  START USER CONFIG SECTION *********************************/

#include "arduino_secrets.h"

#define USER_MQTT_PORT            1883
#define USER_MQTT_CLIENT_NAME     "UVMeter"         // Used to define MQTT topics, MQTT Client ID, and ArduinoOTA
#define MQTT_SUB_TOPIC            "weather/uv_index"

#define ROTATION_STEPS            771               //Defines the number of steps needed to move to max position
#define MAX_UV_INDEX              11

#define STEPPER_PIN_A1           D5
#define STEPPER_PIN_A2           D6
#define STEPPER_PIN_B1           D7
#define STEPPER_PIN_B2           D8
#define STEPPER_TYPE             4
 
/*****************  END USER CONFIG SECTION *********************************/
/*****************  END USER CONFIG SECTION *********************************/
/*****************  END USER CONFIG SECTION *********************************/
/*****************  END USER CONFIG SECTION *********************************/
/*****************  END USER CONFIG SECTION *********************************/

WiFiClient espClient;
PubSubClient client(espClient);
SimpleTimer timer;
AccelStepper stepper = AccelStepper(STEPPER_TYPE, STEPPER_PIN_A1, STEPPER_PIN_A2, STEPPER_PIN_B1, STEPPER_PIN_B2);


//Global Variables
char charPayload[50];

const char* ssid = USER_SSID ; 
const char* password = USER_PASSWORD ;
const char* mqtt_server = USER_MQTT_SERVER ;
const int mqtt_port = USER_MQTT_PORT ;
const char *mqtt_client_name = USER_MQTT_CLIENT_NAME ; 


void setup_wifi() {
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

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
      if (client.connect(mqtt_client_name)) {
        Serial.println("connected");
        client.publish("clients/"USER_MQTT_CLIENT_NAME"/checkIn","Reconnected");
        // ... and resubscribe
        client.subscribe(MQTT_SUB_TOPIC);
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

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  String newTopic = topic;
  Serial.print(topic);
  Serial.print("] ");
  payload[length] = '\0';
  String newPayload = String((char *)payload);
  int intPayload = newPayload.toInt();
  Serial.println(newPayload);
  Serial.println();
  newPayload.toCharArray(charPayload, newPayload.length() + 1);
  if (newTopic == MQTT_SUB_TOPIC) {
      int newPosition = map(constrain(intPayload, 0, MAX_UV_INDEX), 0, MAX_UV_INDEX, 0, ROTATION_STEPS);
      Serial.println("Moving");
      Serial.println(newPosition);
      stepper.enableOutputs();
      stepper.moveTo(newPosition);
  }
  
}

void checkIn() {
  client.publish("clients/"USER_MQTT_CLIENT_NAME"/checkIn","OK"); 
}

//Run once setup
void setup() {
  Serial.begin(115200);
  stepper.setMaxSpeed(300);
  stepper.setAcceleration(300.0);
  delay(5000);
  
  WiFi.mode(WIFI_STA);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  ArduinoOTA.setHostname(USER_MQTT_CLIENT_NAME);
  ArduinoOTA.begin(); 
  delay(10);
  //timer.setInterval(90000, checkIn);
}

void loop() {
  if (!client.connected()) 
  {
    reconnect();
  }
  client.loop();
  ArduinoOTA.handle();
  //timer.run();
  stepper.run();
  if (!stepper.isRunning()) {
    stepper.disableOutputs();
  }
}
