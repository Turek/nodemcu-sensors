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
unsigned long debounceDelay = 500;

// Store our messages here before posting.
char msg[50];

// Define DHT pin and readout variables.
float Temperature;
float Humidity;
#define DHTTYPE DHT22

const uint8_t pins[] = {
  D1,  // Left gate
  D2,  // Right gate
  D5, // Front door
  D7, // Safe
  D4,  // Bell
  D6  // DHT
};

uint8_t state[5];
uint8_t lastState[] = {HIGH,HIGH,HIGH,HIGH,HIGH};
uint8_t lastSentState[5];
unsigned long lastDebounceTime[] = {0,0,0,0,0};
char *sensorNames[] = {
  "Left gate",
  "Right gate",
  "Front door",
  "Safe",
  "Bell",
};
