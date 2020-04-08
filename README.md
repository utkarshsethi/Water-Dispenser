## Automatic Filtered Water Dispenser

### Project Description 

The purpose of this project was to create a water filtration system that dispenses water automatically when a glass is placed under the tap. It should fade on LEDs when dispensing water, and should log how much water I drink and alert me when to change the filter.

(picture link to come)

(video link to come)

### Wiring Diagram and Parts List

[wiring diagram](https://github.com/StorageB/Water-Dispenser/blob/master/wiring-diagram.pdf)

[parts list](https://github.com/StorageB/Water-Dispenser/blob/master/parts-list.md)



### Build Notes and Considerations


#### Valve
1.  The water valve should be a fail close valve. If the power goes out or signal is otherwise lost from the microcontroller, the valve should close or remain closed.
2.  You can find cheaper solenoid valves on Amazon or other sites, but keep in mind the following when selecting a valve:
      - Safety: It needs to be safe to use for beverage applications (should use lead free brass and safe to drink from plastics). 
      - Reliability: This is a commercial valve made specifically for this application. I would not trust a $3 valve from Amazon when installing this permanently in my house.
3.  The sound of the solenoid valve is pretty noticeable. I may consider building an enclosure around it to reduce the noise.
4.  It is a good idea to have a delay after the valve is open before it is allowed to close. This will prevent rapid switching of the valve that may lead to early failure.



#### NeoPixel LED strip

1. Because the ESP module uses 3.3V for the GPIO pins, a level shifter was required for the 5V NeoPixel strip. Although you can get by without a level shifter for most applications, to do very quick changes such as fading all of the LEDs on/off the level shifter was required. Without it, the LED had unstable behavior.

2. Using NeoPixels was overkill for this project, but used mostly for learning purposes. This could have easily been completed with regular LEDs. Maybe I'll add some light animations in the future.

3. Make sure your power supply is large enough to power the LEDs. Adafruit has a good guide for how to power your NeoPixels:

   https://learn.adafruit.com/adafruit-neopixel-uberguide/powering-neopixels



#### Fading LEDs

When fading LEDs with PWM, the light output levels do not scale linearly. Therefore, a logarithm curve is required. This post gives a great explanation on this along with some example Arduino code that I used in this project: 

https://diarmuid.ie/blog/pwm-exponential-led-fading-on-arduino-or-other-platforms



#### IR Sensor

1. Ghost Detection:  Occasionally an IR sensor may give you false triggers based on what the light may be reflecting on. I had this problem but solved it but adding a simple 100 ms delay after it was triggered. After the delay, the systems checks again to see if the sensor is still triggered. 
2. IR sensors do not work well with glass. The sensor had to be positioned to detect a hand holding a glass. This resulted in the sensor not being as accurate and responsive as I'd like.

