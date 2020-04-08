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
#define STASSID "network"   // enter network name
#define STAPSK  "password"  // enter password
#endif
const char* ssid = STASSID;
const char* password = STAPSK;

void setup() {
  Serial.begin(9600);
  
  // OTA code start
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
  // OTA code end

  pinMode(LED_BUILTIN, OUTPUT);     // Initialize on board LED as output  
  pinMode(valve_output, OUTPUT);    // Initialize pin as digital output   (solenoid valve)
  pinMode(ir_input, INPUT);         // Initialize pin as digital input    (infrared sensor)
  pinMode(switch_input, INPUT);     // Initialize pin as digital input    (pushbutton switch)
  
  digitalWrite(LED_BUILTIN, HIGH); // LED off
  digitalWrite(valve_output, LOW); // valve closed

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(led_brightness); // Set BRIGHTNESS (max = 255)

}

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

void turn_on() {
  digitalWrite(LED_BUILTIN, LOW); // LED on
  digitalWrite(valve_output, HIGH); // valve open
  if (light_on==0) {
    fade_in(strip.Color(  0,   0, 255), 2); 
  }
  delay(50);
}

void turn_off() {
  digitalWrite(LED_BUILTIN, HIGH); // LED off
  digitalWrite(valve_output, LOW); // valve closed
  if (light_on==1) {
    fade_out(strip.Color(  0,   0, 255), 3); 
  }
  delay(50);
}







// Rainbow-enhanced theater marquee. Pass delay time (in ms) between frames.
void theaterChaseRainbow(int wait) {
  int firstPixelHue = 0;     // First pixel starts at red (hue 0)
  for(int a=0; a<30; a++) {  // Repeat 30 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in increments of 3...
      for(int c=b; c<strip.numPixels(); c += 3) {
        // hue of pixel 'c' is offset by an amount to make one full
        // revolution of the color wheel (range 65536) along the length
        // of the strip (strip.numPixels() steps):
        int      hue   = firstPixelHue + c * 65536L / strip.numPixels();
        uint32_t color = strip.gamma32(strip.ColorHSV(hue)); // hue -> RGB
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show();                // Update strip with new contents
      delay(wait);                 // Pause for a moment
      firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
    }
  }
}

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(int wait) {
  // Hue of first pixel runs 5 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this outer loop:
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
    for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the strip
      // (strip.numPixels() steps):
      int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the single-argument hue variant. The result
      // is passed through strip.gamma32() to provide 'truer' colors
      // before assigning to each pixel:
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
    strip.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}

// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}

// Theater-marquee-style chasing lights. Pass in a color (32-bit value,
// a la strip.Color(r,g,b) as mentioned above), and a delay time (in ms)
// between frames.
void theaterChase(uint32_t color, int wait) {
  for(int a=0; a<10; a++) {  // Repeat 10 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in steps of 3...
      for(int c=b; c<strip.numPixels(); c += 3) {
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show(); // Update strip with new contents
      delay(wait);  // Pause for a moment
    }
  }
}


/*
  // how to call the different annimations:

  // Fill along the length of the strip in various colors...
  colorWipe(strip.Color(255,   0,   0), 50); // Red
  colorWipe(strip.Color(  0, 255,   0), 50); // Green
  colorWipe(strip.Color(  0,   0, 255), 50); // Blue

  // Do a theater marquee effect in various colors...
  theaterChase(strip.Color(127, 127, 127), 50); // White, half brightness
  theaterChase(strip.Color(127,   0,   0), 50); // Red, half brightness
  theaterChase(strip.Color(  0,   0, 127), 50); // Blue, half brightness

  rainbow(10);             // Flowing rainbow cycle along the whole strip
  theaterChaseRainbow(50); // Rainbow-enhanced theaterChase variant
*/




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

  // object detected
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
  


  
  //delay(300); //note that delay built in to the fade in and fade out functions so this is not needed
  //Serial.println("-");



} // end main loop

