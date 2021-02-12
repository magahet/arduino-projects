#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

/* Put your SSID & Password */
#include "arduino_secrets"

const wifi_ssid[] = SECRET_WIFI_SSID;
const wifi_pass[] = SECRET_WIFI_PASS;

/* Put IP Address details */
IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

ESP8266WebServer server(80);

uint8_t RELAYpin = D2;
bool RELAYstatus = LOW;

uint8_t SENSORpin = D1;
bool SENSORstatus = LOW;

int start;
int wait = 3 * 1000;
int relay_duration = 1 * 1000;
int sensor_threshold = 1 * 5000;
int sensor_duration;


void setup() {
  Serial.begin(115200);
  pinMode(RELAYpin, OUTPUT);
  pinMode(SENSORpin, INPUT);


  WiFi.softAP(wifi_ssid, wifi_pass);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);
  
  server.on("/", handle_OnConnect);
  server.on("/on", handle_on);

  server.onNotFound(handle_NotFound);
  
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  sensor_duration = pulseIn(SENSORpin, HIGH);
  Serial.println(sensor_duration);
  if (sensor_duration > sensor_threshold) {
    RELAYstatus = HIGH;
  }
  
  server.handleClient();
  
  if(RELAYstatus) {
    if (millis() - start > wait) {
      digitalWrite(RELAYpin, HIGH);
      start = millis();
      return;
    }

    if (millis() - start > relay_duration) {
      RELAYstatus = LOW;
      Serial.println("Relay Status: OFF");
      digitalWrite(RELAYpin, LOW);
      start = millis();
      return;
    }
    
  }
}

void handle_OnConnect() {
  Serial.println("Client Connected");
  server.send(200, "text/html", SendHTML()); 
}

void handle_on() {
  RELAYstatus = HIGH;
  Serial.println("Relay Status: ON");
  server.send(200, "text/html", SendHTML()); 
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

String SendHTML(){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>Soak Seat</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr +=".button {display: block;width: 80px;background-color: #1abc9c;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr +="p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<h1>ESP8266 Web Server</h1>\n";
  ptr +="<h3>Using Access Point(AP) Mode</h3>\n";
  ptr +="<a class=\"button\" href=\"/on\">ON</a>\n";
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}
