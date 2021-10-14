#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFi.h>
#define COUNT(a) (sizeof(a) / sizeof(a[0]))

// DFPlayer_Mini Control
#include "DFRobotDFPlayerMini.h"
#define DFPLAYER_SMDLED_PIN_RX 16
#define DFPLAYER_SMDLED_PIN_TX 17
HardwareSerial softwareSerial_channel1(1);
DFRobotDFPlayerMini CPN_DFPlayer;

// BME280
#include <Wire.h>
#include <sSense-BMx280I2C.h>
BMx280I2C::Settings BME280_Settings(
    BME280::OSR_X1,
    BME280::OSR_X1,
    BME280::OSR_X1,
    BME280::Mode_Forced,
    BME280::StandbyTime_1000ms,
    BME280::Filter_Off,
    BME280::SpiEnable_False,
    0x76   // I2C address. I2C specific.
);
BMx280I2C CPN_BME280(BME280_Settings);

// SMD LED
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>   // Required for 16 MHz Adafruit Trinket
#endif
#define SMDLED_CONFIG_DURATION 4000
#define SMDLED_CONFIG_FULL_BRIGHTNESS 255
#define SMDLED_CONFIG_NUMPIXELS 5
#define SMDLED_PIN 27
Adafruit_NeoPixel CPN_SMDLED(SMDLED_CONFIG_NUMPIXELS, SMDLED_PIN, NEO_GRB + NEO_KHZ800);

// FSR 404
#define FSR404_PIN 33

// Flex Resistance
#define FLEX_PIN 32
#define FLEX_VCC 3.3
#define FLEX_FLAT_RESISTANCE 3786
#define FLEX_BEND_RESISTANCE 7181
#define FLEX_DOWNPULL_RESISTANCE 10000
#define FLEX_LENGTH 10.5
#define PI 3.1415926
int Flex_Value = 0;
float Flex_Depth = 0.0;

/*******************************************************************************/
/* DEBUG MACRO */
#define __DEBUG__

#ifdef __DEBUG__
#define DEBUG(...)                  \
    Serial.print("@[SRC]: ");       \
    Serial.print(__FILE__);         \
    Serial.print("\t@[LINE]: ");    \
    Serial.print(__LINE__);         \
    Serial.print("\t@[FUNCTION]:"); \
    Serial.print(__func__);         \
    Serial.print("\t");             \
    Serial.println(__VA_ARGS__);
#else
// else DEBUG is diabled
#define DEBUG(...)
#endif

#define BAUDRATE 115200

/*******************************************************************************/
// Tasks, Function Declaration
// Core 1
void DataUpload();
void SMDLEDControl();

// Core 0
void TaskonCore0(void*);

/*******************************************************************************/
/* Sharing Message : Variables that share between 2 Cores */

/* UNDER CORE 0 */
bool isNewDataComing = false;   // a flag to check whether there's a new data coming

/* UNDER CORE 1 */

/*******************************************************************************/

// Wifi HotSpot
const char* ssid = "realme X3";
const char* password = "0979268400";

void setup() {
    // GLOBAL SETUP

    // Serial Configuration
    Serial.begin(BAUDRATE);

    // // 1. DFPlayer_Mini Configuration
    // softwareSerial_channel1.begin(9600, SERIAL_8N1, DFPLAYER_SMDLED_PIN_RX, DFPLAYER_SMDLED_PIN_TX);   // 1-1 : Start softwareSerial via function begin(BAUDRATE, SERIAL MODE, RX, TX)
    // if (!CPN_DFPlayer.begin(softwareSerial_channel1)) {                                                // 1-2 : Use softwareSerial to communicate with mp3.
    //     Serial.println(CPN_DFPlayer.readType(), HEX);
    //     Serial.println("[STATE] 1. DFPlayer_Mini : FAILED");
    //     while (true)
    //         ;
    // }
    // CPN_DFPlayer.setTimeOut(500);                    // 1-3 : Set Serial Communication TimeOut 500ms
    // CPN_DFPlayer.volume(30);                         // 1-3 : Set Volume, range from 0~30
    // CPN_DFPlayer.EQ(DFPLAYER_EQ_NORMAL);             // 1-3 : Set Different EQ
    // CPN_DFPlayer.outputDevice(DFPLAYER_DEVICE_SD);   // 1-3 : Set Device output, Use SD Card as Default
    // CPN_DFPlayer.play(1);                            // 1-4 : Start Playing 0001.mp3
    // Serial.println("[STATE] 1. DFPlayer_Mini : OK");
    // Serial.print("[INFO] DFPlayer_MIni.State : ");
    // Serial.println(CPN_DFPlayer.readState());
    // Serial.print("[INFO] DFPlayer_MIni.Volume : ");
    // Serial.println(CPN_DFPlayer.readVolume());
    // Serial.print("[INFO] DFPlayer_MIni.FileCounts : ");
    // Serial.println(CPN_DFPlayer.readFileCounts());
    // Serial.print("[INFO] DFPlayer_MIni.CurrentFileNumber : ");
    // Serial.println(CPN_DFPlayer.readCurrentFileNumber());

    // // 2. AD8232
    // // 3. BME280
    // Wire.begin();
    // while (!CPN_BME280.begin()) {
    //     delay(100);
    // }
    // BME280_Settings.tempOSR = BME280::OSR_X4;
    // CPN_BME280.setSettings(BME280_Settings);
    // Serial.println("[STATE] 3. BME280 : OK");

    // // 4. FSR404 Configuration
    // pinMode(FSR404_PIN, INPUT);
    // Serial.println("[STATE] 4. FSR404 : OK");

    // 5. Flex Resiatnce
    pinMode(FLEX_PIN, INPUT);
    Serial.println("[STATE] 5. Flex Resistace : OK");

    // Start Task on Core 0
    // 在 核心0 啟動 任務1
    xTaskCreatePinnedToCore(
        TaskonCore0,   // 任務實際對應的Function
        "Task1",       // 任務名稱
        10000,         // 堆疊空間
        NULL,          // 無輸入值
        0,             // 優先序 0
        NULL,          // 對應的任務變數位址
        0              // 指定在核心0執行
    );
    Serial.println("[STATE] TaskOnCore0 : OK");
}

// Exponential Moving Average
float alpha = 0.6;
float preA = 0.0;

// BME280
int interval = 10;
int times = 0;
float interval_value = 0;
float avg_pres = 0;

void loop() {
    // // Loop for Core 1
    // // Tasks in Core 1
    // // 1: DFPlayer_Mini
    // // 2: AD8232

    // // 3: BME280
    // float temp(NAN), hum(NAN), pres(NAN);
    // BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
    // BME280::PresUnit presUnit(BME280::PresUnit_Pa);

    // CPN_BME280.read(pres, temp, hum, tempUnit, presUnit);
    // times++;
    // interval_value += pres;
    // if (times == interval) {
    //     avg_pres = interval_value / interval;
    //     DEBUG(avg_pres);
    //     interval_value = 0;
    //     times = 0;
    // }

    // // 4: FSR404
    // int FSR404_Pressure = analogRead(FSR404_PIN);
    // DEBUG(FSR404_Pressure);

    // 5: Flex Resistance
    Flex_Value = analogRead(FLEX_PIN);
    // DEBUG(Flex_Value);

    // After all, set the new data coming Flag to true
    isNewDataComing = true;

    delay(500);
}

/*******************************************************************************/

// Tasks in Core 0
// 1: Uploading Data to lineBot server via HTTP Request
// 2: SMD LED Control

void DataUpload() {
    // Send Data with json format to WebServer via Wifi Http Request

    // 2. Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;

        // 3. HTTP Request Configuration
        http.begin("http://click-server-on-heroku.herokuapp.com/esp");   //Specify destination for HTTP request
        http.addHeader("Content-Type", "application/json");              //Specify content-type header

        // TODO : POST CONTENT
        char content[100];
        snprintf_P(
            content,
            COUNT(content),
            PSTR("{\"CPR_Depth\":%02u, \"CPR_Freq\":%02u, \"Breath_Freq\":%02u}"),
            Flex_Depth,
            0,
            0);
        Serial.print("[INFO] ");
        Serial.println(content);

        int httpResponseCode = http.POST(content);   //Send the actual POST request
        // int httpResponseCode = http.GET();

        // 4. Check Response
        if (httpResponseCode > 0) {
            String response = http.getString();   //Get the response to the request

            DEBUG(httpResponseCode);
            DEBUG(response);

            // Serial.println(httpResponseCode);   //Print return code
            // Serial.println(response);           //Print request answer

        } else {
            Serial.print("Error : ");
            Serial.println(httpResponseCode);
        }

        http.end();   //Free resources

    } else {
        Serial.println("Error in WiFi connection");
    }
}

void SMDLEDControl() {
    // 提供 按壓是否太快或太慢的資訊給使用者

    // 1. 由 Flex Sensor 電阻值 估算 按壓深度
    // Read the ADC, and calculate voltage and resistance from it
    float Flex_Voltage = Flex_Value * FLEX_VCC / 4096.0;
    float Flex_Resistance = FLEX_DOWNPULL_RESISTANCE * (FLEX_VCC / Flex_Voltage - 1.0);

    // Use the calculated resistance to estimate the sensor's bend angle:
    int Flex_Angle = map(Flex_Resistance, FLEX_FLAT_RESISTANCE, FLEX_BEND_RESISTANCE, 0, 90.0);
    Flex_Depth = FLEX_LENGTH * sin(Flex_Angle * PI / 180);

    Serial.print("[INFO] Resistance: " + String(Flex_Resistance) + " ohms");
    Serial.print("\tBend: " + String(Flex_Angle) + " degrees");
    Serial.println("\tDepth: " + String(Flex_Depth) + " cm");

    // TODO
    // 依照角度區間 來給予 深度等級
    switch (Flex_Angle / 10) {
        case 0:
            Flex_Depth = 0;
            break;
        case 1:
        case 2:
            Flex_Depth = 1;
            break;
        case 3:
        case 4:
            Flex_Depth = 2;
            break;
        case 5:
        case 6:
            Flex_Depth = 3;
            break;
        case 7:
        case 8:
        case 9:
            Flex_Depth = 4;
            break;
        default:
            break;
    }

    // 2. SMD LED 亮度提示
    CPN_SMDLED.clear();                                    // Set all pixel colors to 'off'
    for (int i = 0; i <= SMDLED_CONFIG_NUMPIXELS; i++) {   // For each pixel...
        // SMD LED 會隨著深度 改變 亮著的 LED 燈顆數
        if (i > Flex_Depth) break;
        CPN_SMDLED.setPixelColor(i, CPN_SMDLED.Color(255, 0, 0));
    }
    CPN_SMDLED.show();   // Send the updated pixel colors to the hardware.
    // delay(SMDLED_CONFIG_DURATION / SMDLED_CONFIG_FULL_BRIGHTNESS);   // Pause before next pass through loop
}

/*******************************************************************************/

void TaskonCore0(void* pvParameters) {
    // Setup for Core 0

    // 1. SMD LED Configuration
    // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
    // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
    clock_prescale_set(clock_div_1);
#endif
    // END of Trinket-specific code.
    CPN_SMDLED.begin();   // INITIALIZE NeoPixel strip object (REQUIRED)
    Serial.println("[STATE] 1. SMD LED: OK");

    // 2. Wifi Connection
    delay(4000);                              // 2-1 : Delay needed before calling the WiFi.begin
    WiFi.begin(ssid, password);               // 2-2 : Connect to Wifi via ssid & password
    while (WiFi.status() != WL_CONNECTED) {   // 2-3 : Check for the Wifi connection
        delay(1000);
        Serial.println("[SETUP] Connecting to WiFi ...");
    }
    Serial.println("[STATE] 2. Wifi : OK");

    // Loop for Core 0
    for (;;) {
        // busy-looSMDLED_PINg until NFC Flag being triggered
        if (isNewDataComing) {
            // do work
            // DataUpload();
            SMDLEDControl();

            // set the NDC Flag to false
            isNewDataComing = false;
        } else {
            delay(1);
        }
    }
}

/*******************************************************************************/