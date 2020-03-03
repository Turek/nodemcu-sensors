// Globals.
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASS;
const char* mqtt_server = MQTT_SERVER;
const char* mqtt_username = MQTT_USER;
const char* mqtt_password = MQTT_PASS;
const uint16_t mqtt_port = MQTT_PORT;

// Millisecond variables, since we are not using delay() in the loop().
unsigned long milliseconds;
unsigned long every5minutes;

// Define debouncer delay.
unsigned long debounceDelay = 50;

// Store our messages here before posting.
char msg[50];

// Define DHT pin and readout variables.
const uint8_t DHTPin = 12; // D6 pin.
float Temperature;
float Humidity;
#define DHTTYPE DHT22

// Bell sensor variables.
const uint8_t bellSensor = 2; // D4 pin.
uint8_t bellSensorState;
uint8_t lastBellSensorState = HIGH;
uint8_t lastSentBellSensorState;
unsigned long lastBellSensorDebounceTime = 0;
