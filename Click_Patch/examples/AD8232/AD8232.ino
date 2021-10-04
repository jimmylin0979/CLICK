/*
 * VARIABLES
 * count: variable to hold count of rr peaks detected in 10 seconds
 * flag: variable that prevents multiple rr peak detections in a single heatbeat
 * hr: HeartRate (initialised to 72)
 * hrv: Heart Rate variability (takes 10-15 seconds to stabilise)
 * instance1: instance when heart beat first time
 * interval: interval between second beat and first beat
 * timer: variable to hold the time after which hr is calculated
 * value: raw sensor value of output pin
 */

#include <Arduino.h>

void setup() {
    // initialize the serial communication:
    Serial.begin(9600);
    pinMode(39, INPUT);   // Setup for leads off detection LO +
    pinMode(36, INPUT);   // Setup for leads off detection LO -
}

void loop() {
    if ((digitalRead(39) == 1) || (digitalRead(36) == 1)) {
        Serial.println("!");
    } else {
        // send the value of analog input 0:
        Serial.println(analogRead(34));
    }
    //Wait for a bit to keep serial data from saturating
}
