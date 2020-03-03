/*
 Sample office automation feeding to my local MQTT server.
 Gathering data from toilet sensors and main gate.

 Developed by Tomasz Turczynski
 - tomasz@turczynski.com
 - https://www.turczynski.com/
 
*/

// Include local files.
#include "config.h"
#include "variables.h"
// Include needed libraries.
#include <PubSubClient.h>
#include <DHT.h>
#include <DHT_U.h>
#include <ESP8266WiFi.h>


int gateSensorLeftState;
int gateSensorRightState;
int frontDoorSensorState;
int safeSensorState;

// Switch sensors
int gateSensorLeft = 5; //D1
int gateSensorRight = 4; //D2
int frontDoorSensor = 14; //D5
int safeSensor = 13; //D7

// Instantiate DHT sensor.
DHT dht(DHTPin, DHTTYPE);

// Instantiate MQTT client.
WiFiClient espClient;
PubSubClient client(espClient);

// The setup routine runs once when you press reset.
void setup() {
  // Initialize serial communication at 9600 bits per second.
  Serial.begin(9600);
  
  // Connect to the WiFi network.
  Serial.println("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  // Wait till we are connected to WiFi network.
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");
  Serial.println(WiFi.localIP()); 

  // Initialize switch sensors.
  pinMode(gateSensorLeft, INPUT);
  pinMode(gateSensorRight, INPUT);
  pinMode(frontDoorSensor, INPUT);
  pinMode(safeSensor, INPUT);
  pinMode(bellSensor, INPUT);

  // Initialize DHT sensor.
  pinMode(DHTPin, INPUT);
  dht.begin();

  // Init MQTT client.
  client.setServer(mqtt_server, mqtt_port);
}

// the loop routine runs over and over again forever:
void loop() {
  milliseconds = millis();
  every5minutes = milliseconds % (1000UL * 60 * 5); // 5 minute interval
  // Execute tasks every 5 minutes.
  if (every5minutes >= 0 && every5minutes < 5) {
    runEvery5minutes();
  }
  // read the input pin:
  gateSensorLeftState = digitalRead(gateSensorLeft);
  gateSensorRightState = digitalRead(gateSensorRight);
  frontDoorSensorState = digitalRead(frontDoorSensor);
  safeSensorState = digitalRead(safeSensor);
  bellSensorState = digitalRead(bellSensor);
  processBellSensor(bellSensorState);
  // print out the state of the button:
//  Serial.print("gateSensorLeftState:");
//  Serial.println(gateSensorLeftState);
//  Serial.print("gateSensorRightState:");
//  Serial.println(gateSensorRightState);
//  Serial.print("frontDoorSensorState:");
//  Serial.println(frontDoorSensorState);
//  Serial.print("safeSensorState:");
//  Serial.println(safeSensorState);
  
  client.loop();
}

void processBellSensor(uint8_t state) {
  // If the switch changed, due to noise or pressing:
  if (state != lastBellSensorState) {
    // reset the debouncing timer
    lastBellSensorDebounceTime = millis();
  }
  if ((millis() - lastBellSensorDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:
    state = digitalRead(bellSensor);
    // if the button state has changed:
    if (state != bellSensorState) {
      bellSensorState = state;
    }
  }
  if (lastSentBellSensorState != bellSensorState) {
    snprintf(msg, 50, "%d", state);
    mqttPush("bellSensorState", msg);
    lastSentBellSensorState = bellSensorState;
  }
}

void runEvery5minutes() {
  // Gets the values of the temperature
  Temperature = dht.readTemperature();
  // Send toilet Temperature.
  snprintf(msg, 50, "%.2f", Temperature);
  mqttPush("toiletTemperature", msg);
  // Gets the values of the humidity 
  Humidity = dht.readHumidity();
  // Send toilet Humidity.
  snprintf(msg, 50, "%.2f", Humidity);
  mqttPush("toiletHumidity", msg);
}

void mqttPush(char* topic, char* payload) {
  if (client.connect("arduinoClient", mqtt_username, mqtt_password)) {
    client.publish(topic, payload);
  }
}
