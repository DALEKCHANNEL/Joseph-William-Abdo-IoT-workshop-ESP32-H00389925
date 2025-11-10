
#include <WiFi.h>
#include <Adafruit_NeoPixel.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>

// --------------------------------------------------------------------------------------------
//        UPDATE CONFIGURATION TO MATCH YOUR ENVIRONMENT
// --------------------------------------------------------------------------------------------

// Add GPIO pins used to connect devices
#define RGB_PIN 14 // GPIO pin the data line of RGB LED is connected to
#define DHT_PIN 27 // GPIO pin the data line of the DHT sensor is connected to

// Specify DHT11 (Blue) or DHT22 (White) sensor
#define DHTTYPE DHT11
#define NEOPIXEL_TYPE NEO_RGB + NEO_KHZ800

// Temperatures to set LED by (assume temp in C)
#define ALARM_COLD 0.0
#define ALARM_HOT 30.0
#define WARN_COLD 10.0
#define WARN_HOT 25.0

#define MQTT_HOST "broker.mqtt.cool"
#define MQTT_PORT 1883
#define MQTT_DEVICEID "esp32-test-01joe"
#define MQTT_USER "" // no need for authentication, for now
#define MQTT_TOKEN "" // no need for authentication, for now
#define MQTT_TOPIC "esp32-test-01joe/evt/status/fmt/json"
#define MQTT_TOPIC_DISPLAY "esp32-test-01joe/fmt/display"
#define MQTT_TOPIC_INTERVAL "esp32-test-01joe/fmt/interval" //new topic added for the subscriber 
                                                            //looking for data from node-red
// Add WiFi connection information
char ssid[] = "Mohammed_5G_joe";  // your network SSID (name)
char pass[] = "Dubai2020";  // your network password


// --------------------------------------------------------------------------------------------
//        SHOULD NOT NEED TO CHANGE ANYTHING BELOW THIS LINE
// --------------------------------------------------------------------------------------------
Adafruit_NeoPixel pixel = Adafruit_NeoPixel(1, RGB_PIN, NEOPIXEL_TYPE);
DHT dht(DHT_PIN, DHTTYPE);

void callback(char* topic, byte* payload, unsigned int length);
WiFiClient wifiClient;
PubSubClient mqtt(MQTT_HOST, MQTT_PORT, callback, wifiClient);

// variables to hold data
StaticJsonDocument<100> jsonDoc;
StaticJsonDocument<100> jsonIncoming;
JsonObject payload = jsonDoc.to<JsonObject>();
JsonObject status = payload.createNestedObject("d");
static char msg[50];

float h = 0.0; // humidity
float t = 0.0; // temperature
unsigned char r = 0; // LED RED value
unsigned char g = 0; // LED Green value
unsigned char b = 0; // LED Blue value
int intervaldelay=1000;
const int loopSteps = 100;
bool ledOverriden = false;

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] : ");
  
  payload[length] = 0; // ensure valid content is zero terminated so can treat as c-string
  Serial.println((char *)payload);

  // Connection code for forum of Part 3 section(Interval)
  String strTopic = String(topic);

  if (strTopic == MQTT_TOPIC_INTERVAL){
    //determine if the MQTT Interval topic is being detected and runs the code below


    DeserializationError err = deserializeJson(jsonIncoming, payload);
    //creates a deserialization enumerated value to detect if the json (payload)
    //has been detected properly and has data


    switch (err.code()){
      //a swtich case was created to determine if the json file has any issues or is 
      //able to be used
      case DeserializationError::Ok:
        Serial.print(F("Deserialization succeeded"));
        //the json file has been detected correctly and can now have the data extracted
        if (jsonIncoming.containsKey("Interval")){
        //if the new interval value detects the key having the phrase interval
        // it will update the intervaldelay value as seen below
          int newinterval = jsonIncoming["Interval"];
          if (newinterval<1){
            newinterval = 1;
            //this is added to avoid the values going below 0 or into the negatives
          }
          Serial.print("The new interval has been updated to ");
          Serial.print(newinterval);
          Serial.print("seconds()");
          Serial.print(intervaldelay);
          Serial.print("ms)");
          //The below code was multiplied by 1000 to make the delay equal to 10 at the end
          intervaldelay = newinterval*1000;

        }

        break;
        //other deserialization checks that determine if the incoming message is broken, missing or corrupted
      case DeserializationError::EmptyInput:
        Serial.print(F("Deserialization has no inpurt"));
        break;
      case DeserializationError::IncompleteInput:
        Serial.print(F("Deserialization has incomplete data"));
        break;
      case DeserializationError::InvalidInput:
        Serial.print(F("Deserialization has invalid data"));
        break;
      case DeserializationError::NoMemory:
        Serial.print(F("Deserialization has invalid memory"));
        break;
      default:
        Serial.print(F("Deserialization has failed"));
        break;
    }
  }
  else if(strTopic == MQTT_TOPIC_DISPLAY){
      //determine if the MQTT Interval topic is being detected and runs the code below


    DeserializationError err = deserializeJson(jsonIncoming, payload);
    //creates a deserialization enumerated value to detect if the json (payload)
    //has been detected properly and has data


    switch (err.code()){
      //a swtich case was created to determine if the json file has any issues or is 
      //able to be used
      case DeserializationError::Ok:{
        Serial.print(F("Deserialization succeeded"));
        //the json file has been detected correctly and can now have the data extracted
        //gets the three color data values coming from the injection node on Node-RED to change the LED color
        int red = jsonIncoming["r"];
        int green = jsonIncoming["g"];
        int blue = jsonIncoming["b"];
        //prints the color values seperately
        Serial.println("Setting LED to Red:");
        Serial.println(red);
        Serial.println("Setting LED to Green:");
        Serial.println(green);
        Serial.println("Setting LED to Blue:");
        Serial.println(blue);
        //sets the new color values gotten from the injection code temporarily
        pixel.setPixelColor(0, red, green, blue);
        //shows the color values
        pixel.show();
        //an if statement was added as a way to make the code switch from the temperature measurement led code to
        //the new node-red injection code
        ledOverriden = true;
        break;
      }
       //other deserialization checks that determine if the incoming message is broken, missing or corrupted
      case DeserializationError::EmptyInput:
        Serial.print(F("Deserialization has no inpurt"));
        break;
      case DeserializationError::IncompleteInput:
        Serial.print(F("Deserialization has incomplete data"));
        break;
      case DeserializationError::InvalidInput:
        Serial.print(F("Deserialization has invalid data"));
        break;
      case DeserializationError::NoMemory:
        Serial.print(F("Deserialization has invalid memory"));
        break;
      default:
        Serial.print(F("Deserialization has failed"));
        break;
    }
  }
}
void setup()
{
  // Start serial console
  Serial.begin(115200);
  Serial.setTimeout(2000);
  while (!Serial) { }
  Serial.println();
  Serial.println("ESP8266 Sensor Application");

  // Start WiFi connection
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi Connected");

  // Start connected devices
  dht.begin();
  pixel.begin();
    // Connect to MQTT broker
  if (mqtt.connect(MQTT_DEVICEID, MQTT_USER, MQTT_TOKEN)) {
    Serial.println("MQTT Connected");
    mqtt.subscribe(MQTT_TOPIC_DISPLAY);
    mqtt.subscribe(MQTT_TOPIC_INTERVAL);

  } else {
    Serial.println("MQTT Failed to connect!");
    ESP.restart();
  }
}
void loop()
{
  mqtt.loop();
  while (!mqtt.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqtt.connect(MQTT_DEVICEID, MQTT_USER, MQTT_TOKEN)) {
      Serial.println("MQTT Connected");
      mqtt.subscribe(MQTT_TOPIC_DISPLAY);
      mqtt.subscribe(MQTT_TOPIC_INTERVAL);
      mqtt.loop();
    } else {
      Serial.println("MQTT Failed to connect!");
      delay(5000);
    }
  }
  
  h = dht.readHumidity();
  t = dht.readTemperature(); // uncomment this line for Celsius
  // t = dht.readTemperature(true); // uncomment this line for Fahrenheit

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
  } else {
    // Set RGB LED Colour based on temp and if the led has not been overriden by the node-red code
    if (!ledOverriden){
    b = (t < ALARM_COLD) ? 255 : ((t < WARN_COLD) ? 150 : 0);
    r = (t >= ALARM_HOT) ? 255 : ((t > WARN_HOT) ? 150 : 0);
    g = (t > ALARM_COLD) ? ((t <= WARN_HOT) ? 255 : ((t < ALARM_HOT) ? 150 : 0)) : 0;
    pixel.setPixelColor(0, r, g, b);
    pixel.show();
    }

    // Print Message to console in JSON format
    status["temp"] = t;
    status["humidity"] = h;
    serializeJson(jsonDoc, msg, 50);
    Serial.println(msg);
    Serial.print(intervaldelay);
    Serial.print("ms)");
    if (!mqtt.publish(MQTT_TOPIC, msg)) {
      Serial.println("MQTT Publish failed");
    }
  }
  int steps = intervaldelay / loopSteps;
  if (steps<1){
    steps =10;
  }
  for (int i = 0; i < steps; i++) {
    mqtt.loop();
    delay(intervaldelay);
  }
}
