#include <Wire.h>
#include <sSense-BMx280I2C.h>
#define SERIAL_SPEED 115200

BMx280I2C::Settings settings(
    BME280::OSR_X1,
    BME280::OSR_X1,
    BME280::OSR_X1,
    BME280::Mode_Forced,
    BME280::StandbyTime_1000ms,
    BME280::Filter_Off,
    BME280::SpiEnable_False,
    0x76   // I2C address. I2C specific.
);

BMx280I2C ssenseBMx280(settings);

void setup() {
    delay(5000);
    DebugPort.begin(SERIAL_SPEED);

    while (!DebugPort) {
    }
    //DebugPort.println("s-Sense BME/BMP280 I2C sensor.");

    Wire.begin();
    while (!ssenseBMx280.begin()) {
        //DebugPort.println("Could not find BME/BMP280 sensor!");
        delay(1000);
    }

    switch (ssenseBMx280.chipModel()) {
        case BME280::ChipModel_BME280:
            //DebugPort.println("Found BME280 sensor! Humidity available.");
            break;
        case BME280::ChipModel_BMP280:
            //DebugPort.println("Found BMP280 sensor! No Humidity available.");
            break;
        default:
            DebugPort.println("");
    }

    // Change some settings before using.
    settings.tempOSR = BME280::OSR_X4;

    ssenseBMx280.setSettings(settings);
}

void loop() {
    printBMx280Data(&DebugPort);
    delay(1);
}

int interval = 10;
int times = 0;
float interval_value = 0;
float avg_pres = 0;
void printBMx280Data(Stream* client) {
    float temp(NAN), hum(NAN), pres(NAN);

    BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
    BME280::PresUnit presUnit(BME280::PresUnit_Pa);

    ssenseBMx280.read(pres, temp, hum, tempUnit, presUnit);
    times++;
    //Serial.println(pres);
    interval_value += pres;
    if (times == interval) {
        avg_pres = interval_value / interval;
        client->println(avg_pres);
        interval_value = 0;
        times = 0;
    }
    //client->print("Temp: ");
    //client->print(temp);
    //client->print(" "+ String(tempUnit == BME280::TempUnit_Celsius ? 'C' :'F'));
    //client->print("\t\tHumidity: ");
    //client->print(hum);
    //client->print("% RH");
    //client->print("\t\tPressure: ");

    //client->println(" Pa");
}
