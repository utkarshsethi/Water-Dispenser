## Automatic Filtered Water Dispenser

### Project Description 

(work in progress)

The purpose of this project was to create a water filtration system that dispenses water automatically when a glass is placed under the tap.



Goals

1. Detect when a glass is under the tap and dispense water automatically. When the glass is pulled back, the system should turn off and allow water to finish draining into the glass.
2. Log water usage to keep track of how much water I am drinking each day.
3. Alert me when it is time to change the filter



The wiring diagram can be found [here](https://github.com/StorageB/Water-Dispenser/blob/master/Wiring-diagram.pdf)

A complete parts list can be found [here](https://github.com/StorageB/Water-Dispenser/blob/master/Parts-List.md)

(pictures go here)

### Build Notes and Considerations


#### Valve
1. When choosing a valve, make sure it is a fail close valve. If the power goes out or it loses signal from the microcontroller, the valve should close or remain closed
2. You can find cheaper cheaper solenoid valves on Amazon or other sites, but keep in mind the following
      - Safety: It needs to be safe to use for beverage applications (need to use lead free brass and safe to use plastics). 
      - Reliability: This is a commercial valve made specifically for this application. I would not trust a $3 valve from Amazon when installing this permanently in my house.






