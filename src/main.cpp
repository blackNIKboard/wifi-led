#include <Arduino.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <WebServer.h>
#include <WiFi.h>

int leds [] = { 16, 22, 19 }; // R-G-B

int brightness = 0;
int brightStep = 1;

const int ledPin = 16;  // 16 corresponds to GPIO16

WebServer server(80);

String getMIME(const String& filename)
{
  if (filename.endsWith(".html"))
    return "text/html";
  else if (filename.endsWith(".css"))
    return "text/css";
  else if (filename.endsWith(".js"))
    return "application/javascript";
  else if (filename.endsWith(".ico"))
    return "image/x-icon";
  else if (filename.endsWith(".jpg"))
    return "image/jpeg";
  else if (filename.endsWith(".png"))
    return "image/png";
  return "text/plain";
}

bool handlePage(const String& filePath) {
  String contentType = getMIME(filePath);
  if (SPIFFS.exists(filePath)) {
    File file = SPIFFS.open(filePath, "r");
    server.streamFile(file, contentType);
    file.close();
    return true;
  }
  else {
    String err = "File " + filePath + " not found!";
    Serial.println(err);
    return false;
  }
}

void initWiFi(WebServer& server) {
  File configFile = SPIFFS.open("/config/wifi.json", "r");
  const char *ssid = "", *pass = "";
  if (configFile) {
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, configFile);
    if (error) {
      Serial.print("Failed to read a config file: ");
      Serial.println(error.c_str());
    } else {
      ssid = doc["ssid"];
      pass = doc["password"];
      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid, pass);
      unsigned long startTime = millis();
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        if ((unsigned long)(millis() - startTime) >= 7000) {
          Serial.println("Error while try to connect to WI-FI!");
        }
      }
    }
  } else {
    Serial.println("Config file was not found!");
  }
  Serial.println("Starting the webserver on " + WiFi.localIP());
  server.begin();
}

void setLED(char mark, int level) {
  int selectedPin = 0;
  switch (mark) {
  case 'G':
    selectedPin = leds[1];
    break;
  case 'B':
    selectedPin = leds[2];
    break;
  default:
    break;
  }
  analogWrite(selectedPin, level);   
}

void setup() {
  Serial.begin(115200);

  SPIFFS.begin();
  initWiFi(server);

  server.on("/", []() {
      handlePage("/templates/mainpage.html");
    });
}
 
void loop() {
  // increase the LED brightness
  brightness += brightStep;

  Serial.printf("setting brightness to: %d\n", brightness);

  analogWrite(LED_BUILTIN, brightness);
  setLED('R', brightness);
  // analogWrite(16, 255);


  if ( brightness <= 0 || brightness >= 255 ) {
    brightStep = -brightStep;
  }
  
  delay(10);
}
