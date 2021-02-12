#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <Wire.h>
#include <Adafruit_MLX90614.h>

#define subscribe_topic "home/kitchen/stove_temp/set"
#define AVAILABILITY_TOPIC "home/kitchen/stove_temp/availability"
#define SCL       D1
#define SDA       D2
// send update every 10 min
#define INTERVAL 60000
#define WATCH_TEMP 100

// Update these with values suitable for your network.
#include "arduino_secrets"

const wifi_ssid[] = SECRET_WIFI_SSID;
const wifi_pass[] = SECRET_WIFI_PASS;
const mqtt_server[] = SECRET_MQTT_SERVER;

WiFiClient espClient;
PubSubClient client(espClient);
char msg[50];
int lastTemp = 0;
unsigned long last, current;

Adafruit_MLX90614 mlx = Adafruit_MLX90614();


void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str(), AVAILABILITY_TOPIC, 1, true, "offline")) {
      Serial.println("connected");
      client.publish(AVAILABILITY_TOPIC, "online", true);
      client.subscribe(subscribe_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  mlx.begin();
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);

  last = millis() - (2 * INTERVAL);
  checkState();
}

void checkState() {
  int temp = int(mlx.readObjectTempF());
  current = millis();
  if (temp != lastTemp || (temp > WATCH_TEMP && current - last > INTERVAL)) {
    snprintf(msg, 50, "%d", temp);
    Serial.println(msg);
    client.publish("home/kitchen/stove_temp/state", msg);
    lastTemp - temp;
    last = current;
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  checkState();
  delay(5000);
}
