#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFi.h>

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

// Tasks in Core 1
void DataUpload();
void SMDLEDControl();

// Tasks in Core 0
void TaskonCore0(void*);
/*******************************************************************************/
/* Public Variable : will share between 2 cores */

/* UNDER CORE 0 */

bool isNewDataComing = false;

/* UNDER CORE 1 */

// Wifi HTTP Request
const char* ssid = "realme X3";
const char* password = "0979268400";

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

void setup() {
    // GLOBAL SETUP

    // Serial Configuration
    Serial.begin(BAUDRATE);

    // Wifi Connection
    delay(4000);   //Delay needed before calling the WiFi.begin
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {   //Check for the connection
        delay(1000);
        Serial.println("Connecting to WiFi..");
    }
    Serial.println("Connected to the WiFi network");

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
}

// Exponential Moving Average
float alpha = 0.6;
float preA = 0.0;

void loop() {
    // Loop for Core 1
    // Tasks in Core 1
    // 1: Flex Resistance
    // 2: AD8232
    // 3: BME280
    // 4: FSR404
}

/*******************************************************************************/