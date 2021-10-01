#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFi.h>

// DFPlayer_Mini Control
#include "DFRobotDFPlayerMini.h"

/*******************************************************************************/
/* DEBUG MACRO */
#define __DEBUG__

#ifdef __DEBUG__
#define DEBUG(...)                   \
    Serial.println(__VA_ARGS__);     \
    Serial.print(" @ [SRC]:      "); \
    Serial.println(__FILE__);        \
    Serial.print(" @ [LINE]:     "); \
    Serial.println(__LINE__);        \
    Serial.print(" @ [FUNCTION]: "); \
    Serial.println(__func__);
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

// DFPlayer_Mini
#define DFPLAYER_RX 16
#define DFPLAYER_TX 17
HardwareSerial softwareSerial_channel1(1);
DFRobotDFPlayerMini DFPlayer;

// Wifi HotSpot
const char* ssid = "realme X3";
const char* password = "0979268400";

void setup() {
    // GLOBAL SETUP

    // Serial Configuration
    Serial.begin(BAUDRATE);

    // 1. DFPlayer_Mini Configuration
    softwareSerial_channel1.begin(9600, SERIAL_8N1, DFPLAYER_RX, DFPLAYER_TX);   // 1-1 : Start softwareSerial via function begin(BAUDRATE, SERIAL MODE, RX, TX)
    if (!DFPlayer.begin(softwareSerial_channel1)) {                              // 1-2 : Use softwareSerial to communicate with mp3.
        Serial.println(DFPlayer.readType(), HEX);
        Serial.println("[STATE] 1. DFPlayer_Mini : FAILED");
        while (true)
            ;
    }
    DFPlayer.setTimeOut(500);                    // 1-3 : Set Serial Communication TimeOut 500ms
    DFPlayer.volume(30);                         // 1-3 : Set Volume, range from 0~30
    DFPlayer.EQ(DFPLAYER_EQ_NORMAL);             // 1-3 : Set Different EQ
    DFPlayer.outputDevice(DFPLAYER_DEVICE_SD);   // 1-3 : Set Device output, Use SD Card as Default
    DFPlayer.play(1);                            // 1-4 : Start Playing 0001.mp3
    Serial.println("[STATE] 1. DFPlayer_Mini : OK");
    Serial.print("[INFO] DFPlayer_MIni.State : ");
    Serial.println(DFPlayer.readState());
    Serial.print("[INFO] DFPlayer_MIni.Volume : ");
    Serial.println(DFPlayer.readVolume());
    Serial.print("[INFO] DFPlayer_MIni.FileCounts : ");
    Serial.println(DFPlayer.readFileCounts());
    Serial.print("[INFO] DFPlayer_MIni.CurrentFileNumber : ");
    Serial.println(DFPlayer.readCurrentFileNumber());

    // 2. Wifi Connection
    delay(4000);                              // 2-1 : Delay needed before calling the WiFi.begin
    WiFi.begin(ssid, password);               // 2-2 : Connect to Wifi via ssid & password
    while (WiFi.status() != WL_CONNECTED) {   // 2-3 : Check for the Wifi connection
        delay(1000);
        Serial.println("[SETUP] Connecting to WiFi ...");
    }
    Serial.println("[STATE] 2. Wifi : OK");

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

void loop() {
    // Loop for Core 1
    // Tasks in Core 1
    // 1: DFPlayer_Mini
    // 2: AD8232
    // 3: BME280
    // 4: FSR404
    // 5: Flex Resistance
    //
}

/*******************************************************************************/

// Tasks in Core 0
// 1: Uploading Data to lineBot server via HTTP Request
// 2: SMD LED Control

void DataUpload() {
    // Send Data with json format to WebServer via Wifi Http Request

    // // 1. Check isNewDataComing Flag first
    // // DO NOT send data if no new data coming
    // if (isNewDataComing) {
    //     return;
    // }

    // 2. Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;

        // 3. HTTP Request Configuration
        http.begin("http://click-server-on-heroku.herokuapp.com/esp");   //Specify destination for HTTP request
        http.addHeader("Content-Type", "application/json");              //Specify content-type header

        // TODO : POST CONTENT
        String content = "{\"id\":\"1\"}";
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
}

/*******************************************************************************/

void TaskonCore0(void* pvParameters) {
    // Setup for Core 0
    /* Code */

    // Loop for Core 0
    for (;;) {
        // busy-looping until NFC Flag being triggered
        if (isNewDataComing) {
            // do work
            DataUpload();
            SMDLEDControl();

            // set the NDC Flag to false
            isNewDataComing = false;
        }
    }
}

/*******************************************************************************/