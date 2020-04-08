#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#define valve_output   D1   // valve output pin
#define ir_input       D5   // ir sensor input piin
#define switch_input   D7   // pushbutton switch input pin
#define led_pin        D2   // NeoPixel strip signal pin
#define led_count      8    // number of LEDs in NeoPixel strip
#define led_brightness 200  // NeoPixel brightness (max = 255)
#define pwmIntervals   100  // number of intervals in the fade in/out for loops

int ir_state;               // state of IR sensor - LOW if object detected, HIGH if no object detected
int switch_state;           // state of pushbutton switch - HIGH if pressed, LOW if not pressed
int light_on = 0;           // state of NeoPixel strip - 1 if on, 0 if off
int ir_input_delay = 100;   // how long to wait once an object is detected by IR sensor - this prevents ghost inputs 

float R = (pwmIntervals * log10(2))/(log10(255)); // used to calculate the 'R' value for fading LEDs

// Declare NeoPixel strip object:
Adafruit_NeoPixel strip(led_count, led_pin, NEO_GRB + NEO_KHZ800);

// Required for OTA programming
#ifndef STASSID
#define STASSID "network name"  // enter network name
#define STAPSK  "password"  	// enter password
#endif
const char* ssid = STASSID;
const char* password = STAPSK;

void setup() {
  Serial.begin(9600);
  
  // Required for OTA programming
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);
  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");
  // No authentication by default
  // ArduinoOTA.setPassword("admin");
  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  

  // Setup
  pinMode(LED_BUILTIN, OUTPUT);    // Initialize on board LED as output  
  pinMode(valve_output, OUTPUT);   // Initialize pin as digital output   (solenoid valve)
  pinMode(ir_input, INPUT);        // Initialize pin as digital input    (infrared sensor)
  pinMode(switch_input, INPUT);    // Initialize pin as digital input    (pushbutton switch)
  
  digitalWrite(LED_BUILTIN, HIGH); // LED off
  digitalWrite(valve_output, LOW); // valve closed

  strip.begin();           		// INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            		// Turn OFF all pixels ASAP
  strip.setBrightness(led_brightness); 	// Set BRIGHTNESS (max = 255)

}

// Fade LEDs on
void fade_in(uint32_t color, int wait) {
  int brightness = 0;
  for(int i=0; i<=pwmIntervals; i++){
    brightness = pow (2, (i / R)) - 1;
    for(int j=0; j<strip.numPixels(); j++) {
      strip.setPixelColor(j,0,0,brightness);
    }
    strip.show();
    delay(wait);
  }
  light_on=1;
}

// Fade LEDs off
void fade_out(uint32_t color, int wait) {
  int brightness = 255;
  for(int i=pwmIntervals; i>=0; i--){
    brightness = pow (2, (i / R)) - 1;
    for(int j=0; j<strip.numPixels(); j++) {
      strip.setPixelColor(j,0,0,brightness);
    }
    strip.show();
    delay(wait);
  }
  strip.clear();
  strip.show();
  light_on=0;
}

// Open valve and turn on LEDs
void turn_on() {
  digitalWrite(LED_BUILTIN, LOW); // LED on
  digitalWrite(valve_output, HIGH); // valve open
  if (light_on==0) {
    fade_in(strip.Color(  0,   0, 255), 5); 
  }
  delay(50);
}

// Close valve and turn off LEDs
void turn_off() {
  digitalWrite(LED_BUILTIN, HIGH); // LED off
  digitalWrite(valve_output, LOW); // valve closed
  if (light_on==1) {
    fade_out(strip.Color(  0,   0, 255), 5); 
  }
  delay(50);
}


void loop() {
  ArduinoOTA.handle();

  ir_state = digitalRead(ir_input);
  switch_state = digitalRead(switch_input);
  
  // button pressed
  while (switch_state == HIGH) {
    turn_on();
    switch_state = digitalRead(switch_input);
    if (switch_state == LOW) {
      turn_off();
    }
  }

  // object detected from IR sensor
  while (ir_state == LOW) {
    delay(ir_input_delay);
    ir_state = digitalRead(ir_input);
    if (ir_state == LOW) {
      turn_on();
    }
    else if (ir_state == HIGH) {
      turn_off();
    }
  }
  
} //end loop()

