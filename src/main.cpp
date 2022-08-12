#include <Arduino.h>
#include <WiFi.h>

/***
 * LED Blinking
 * Code Written by TechMartian
 */
int leds [] = { 16, 22, 19 }; // R-G-B
// int led = 0;

int brightness = 0;
int brightStep = 1;

// the number of the LED pin
const int ledPin = 16;  // 16 corresponds to GPIO16

WiFiServer server(80);

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin("KONKITAYSKIY", "sahdia7shf7ase83");
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

void setLED(char mark, int level){
  int selectedPin = 0;
  switch (mark)
  {
  // case 'R':
  //   selectedPin = leds[0];
  //   break;
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

// void setup() {
//   Serial.begin(115200);
//   initWiFi();
//   // pinMode (leds[0], OUTPUT);
//   // pinMode (leds[1], OUTPUT);
//   // pinMode (leds[2], OUTPUT);
// }
// void loop() {
//   // if (led > 2)
//   // {
//   //   led = 0;
//   // }

//   // int cur_led = leds[led];
  
//   // digitalWrite (cur_led, HIGH);	// turn on the LED
//   // delay(500);	// wait for half a second or 500 milliseconds
//   // digitalWrite (cur_led, LOW);	// turn off the LED
//   // delay(500);	// wait for half a second or 500 milliseconds

//   // led++;

  
// }
void setup(){
  Serial.begin(115200);
}
 
void loop(){
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
