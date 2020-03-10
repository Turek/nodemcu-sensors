/*
 Sample office automation feeding to my local MQTT server.
 Gathering data from few sensors around the toilet area.

 Developed by Tomasz Turczynski
 - tomasz@turczynski.com
 - https://www.turczynski.com/
 
*/

// Include needed libraries.
#include <HashMap.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <DHT_U.h>
#include <ESP8266WiFi.h>

// Include local files.
#include "config.h"
#include "variables.h"

// Instantiate DHT sensor.
DHT dht(pins[5], DHTTYPE);
// Instantiate WiFi client.
WiFiClient espClient;
// Instantiate MQTT client.
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
  for (byte i = 0; i < sizeof(pins); i = i + 1) {
    pinMode(pins[i], INPUT);
    Serial.println(i);
  }

  // Initialize DHT sensor.
  dht.begin();

  // Init MQTT client.
  client.setServer(mqtt_server, mqtt_port);
}

/** 
 * The loop routine runs over and over again forever.
 */
void loop() {
  milliseconds = millis();
  every5minutes = milliseconds % (1000UL * 60 * 5); // 5 minute interval

  // Execute tasks every 5 minutes.
  if (every5minutes >= 0 && every5minutes < 5) {
    runEvery5minutes();
  }

  // Process sensor states.
  for (byte i = 0; i < sizeof(pins) - 1; i = i + 1) {
    state[i] = digitalRead(pins[i]);
    processSensor(i, state[i]);
  }

  client.loop();
}

/**
 * Process and send Bell sensor readout.
 * 
 * @param uint8_t state
 * @return bool
 */
bool processSensor(byte sensorId, uint8_t currentState) {
  // If the switch changed, due to noise or pressing:
  if (currentState != lastState[sensorId]) {
    // reset the debouncing timer
    lastDebounceTime[sensorId] = millis();
  }
  if ((millis() - lastDebounceTime[sensorId]) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:
    currentState = digitalRead(pins[sensorId]);
    // if the button state has changed:
    if (currentState != state[sensorId]) {
      state[sensorId] = currentState;
    }
  }
  if (lastSentState[sensorId] != state[sensorId]) {
    snprintf(msg, 50, "%d", currentState);
    mqttPush(sensorNames[sensorId], msg);
    lastSentState[sensorId] = state[sensorId];

    return true;
  }
  
  return false;
}

/**
 * All the stuff that needs to run every 5 minutes.
 */
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

/**
 * Send data to MQTT server.
 * 
 * @param char topic
 * @param char payload
 */
void mqttPush(char* topic, char* payload) {
  if (client.connect("arduinoClient", mqtt_username, mqtt_password)) {
    client.publish(topic, payload);
  }
}
