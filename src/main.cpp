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

String getMIME(const String filename)
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

bool handlePage(const String filePath) {
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

void initWiFi() {
  File configFile = SPIFFS.open("/config/wifi.json", FILE_READ);
  const char *ssid = "", *pass = "";
  if (configFile) {
    StaticJsonDocument<200> doc;
    String buf = configFile.readString();
    Serial.println(buf);
    DeserializationError error = deserializeJson(doc, buf);
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
        if ((unsigned long)(millis() - startTime) >= 7000) {
          Serial.println("\nError while try to connect to WI-FI!");
        }
      }
    }
  } else {
    Serial.println("Config file was not found!");
  }
  Serial.println("\nStarting webserver on " + WiFi.localIP().toString());
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
  initWiFi();

  server.on("/", []() {
      handlePage("/templates/index.html");
  });

  server.onNotFound([]() {
      if (!handlePage(server.uri()))
        server.send(404, "text/plain", "404: Not Found");
    });

   server.begin();
}
 
void loop() {

  server.handleClient();
  // increase the LED brightness
  brightness += brightStep;

  // Serial.printf("setting brightness to: %d\n", brightness);

  analogWrite(LED_BUILTIN, brightness);
  setLED('R', brightness);
  // analogWrite(16, 255);


  if ( brightness <= 0 || brightness >= 255 ) {
    brightStep = -brightStep;
  }
  
  delay(10);
}
