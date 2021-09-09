#include <ADXL335.h>
#include <Arduino.h>

/* Serial 傳輸參數 設定 */
#define BAUDRATE 115200
#define DIMENSION 3

typedef short int16_t;

/*******************************************************************************/
void initData();

// Accelerator
ADXL335 accelerometer;

// Acceleration History
unsigned long curTimeStamp = 0, preTimeStamp = 0;
float curA[DIMENSION];

void setup() {
    // Serial Configuration
    Serial.begin(BAUDRATE);

    //
    accelerometer.begin();

    //
    initData();
}

void loop() {
    curTimeStamp = millis();
    accelerometer.getAcceleration(curA + 0, curA + 1, curA + 2);

    // serial print current time stamp
    Serial.print(curTimeStamp);

    // and accelerator input
    // Serial.print(",");
    // Serial.print(curA[0]);
    // Serial.print(",");
    // Serial.print(curA[1]);
    Serial.print(",");
    Serial.println(curA[2]);
}

/*******************************************************************************/

void initData() {
    for (int dim = 0; dim < DIMENSION; dim++) {
        curA[dim] = 0.0;
    }
}
/*******************************************************************************/