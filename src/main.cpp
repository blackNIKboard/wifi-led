#include <Arduino.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <WebServer.h>
#include <WiFi.h>

#define RED 16
#define GREEN 22
#define BLUE 19

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
  int selectedPin = RED;
  switch (mark) {
  case 'G':
    selectedPin = GREEN;
    break;
  case 'B':
    selectedPin = BLUE;
    break;
  default:
    break;
  }
  analogWrite(selectedPin, level);   
}

void setColor(int red, int green, int blue) {
  analogWrite(RED, red);
  analogWrite(GREEN, green);
  analogWrite(BLUE, blue);
}

void setup() {
  Serial.begin(115200);

  SPIFFS.begin();
  initWiFi();

  server.on("/api/color", HTTP_GET, []() {
    server.send(200, "text/plain", "OK");
  });

  server.on("/api/change", HTTP_POST, []() {
    StaticJsonDocument<200> doc;
    String data = server.arg("color");
    DeserializationError error = deserializeJson(doc, data);
    if (error) {
      Serial.println("Failed to deseriakize data for /api/change.");
    }
    else {
      setColor(static_cast<int>(doc["red"]),
               static_cast<int>(doc["green"]),
               static_cast<int>(doc["blue"]));
      server.send(201, "text/plain", "OK");
    }
  });

  server.onNotFound([]() {
      if (!handlePage(server.uri()))
        server.send(404, "text/plain", "404: Not Found");
    });

   server.begin();
}
 
void loop() {
  server.handleClient();
}
