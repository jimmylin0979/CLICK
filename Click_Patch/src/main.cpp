#include <A2DManager.h>
#include <ADXL335.h>
#include <Arduino.h>
#include <HTTPClient.h>
#include <Vector.h>
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

void initData();

// Tasks in Core 1
void Filter();
void SendData();

// Tasls in Core 0
void TaskonCore0(void*);
void A2D();
void LEDControl();

/*******************************************************************************/
/* Public Variable : will share between 2 cores */

/* UNDER CORE 0 */

bool isNewDataComing = false;

/* UNDER CORE 1 */

// Acceleration History
#define ACCUMULATED_AMOUNT 1
float As[ACCUMULATED_AMOUNT], curA;
int accumulated_idx = 0;
// Acceleration TimeStamp
unsigned long curTimeStamp = 0, preTimeStamp = 0;

// Wifi HTTP Request
const char* ssid = "realme X3";
const char* password = "0979268400";

/*******************************************************************************/
// Tasks in Core 1
// 1: Data COllection
// 2: Web Server Data Transfer

/* Acceleration */
// Accelerator
ADXL335 accelerometer;

// Exponential Moving Average
float alpha = 0.6;
float preA = 0.0;

void initData() {
    for (int i = 0; i < ACCUMULATED_AMOUNT; i++) {
        As[i] = 0.0;
    }
}

void Filter() {
    curTimeStamp = millis();

    // 1. Collect Data from ADXL
    // 2. Mean Filter
    // take 10 sample for acceleration's average
    accumulated_idx = 0;
    while (accumulated_idx < ACCUMULATED_AMOUNT) {
        // accelerometer.getAcceleration(curA + 0, curA + 1, curA + 2);
        // we only care about acceleration on Z-Axis
        accelerometer.getAcc_Z(As + accumulated_idx);
        curA += As[accumulated_idx];
        accumulated_idx++;
    }
    curA = curA / ACCUMULATED_AMOUNT;

    // 3. Exponential Moving Average
    curA = curA * alpha + (1.0 - alpha) * preA;

    // 4. stored the value into IntegralInfo
    IntegralInfo* info = (IntegralInfo*)malloc(sizeof(*info));
    info->timeStamp = curTimeStamp;
    info->value = curA;

    // 5. set the isNewDataComing flag to true, let the Task on Core 0 enable to work
    isNewDataComing = true;

    // // serial print current time stamp
    // Serial.print(curTimeStamp);

    // // and accelerator input
    // Serial.print(",");
    // Serial.println(curA[2]);
}

void SendData() {
    // Send Data with json format to WebServer via Wifi Http Request

    // 1. Check isNewDataComing Flag first
    // DO NOT send data if no new data coming
    if (isNewDataComing) {
        return;
    }

    // 2. Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;

        // 3. HTTP Request Configuration
        http.begin("http://click-server-on-heroku.herokuapp.com/esp");   //Specify destination for HTTP request
        http.addHeader("Content-Type", "application/json");              //Specify content-type header

        // TODO : POST CONTENT
        int httpResponseCode = http.POST("POSTING from ESP32");   //Send the actual POST request
        // int httpResponseCode = http.GET();

        // 4. Check Response
        if (httpResponseCode > 0) {
            String response = http.getString();   //Get the response to the request

            Serial.println(httpResponseCode);   //Print return code
            Serial.println(response);           //Print request answer

        } else {
            Serial.print("Error : ");
            Serial.println(httpResponseCode);
        }

        http.end();   //Free resources

    } else {
        Serial.println("Error in WiFi connection");
    }
}

/*******************************************************************************/
// Tasks in Core 0
// 1: ADXL335 Double Integration
// 2: Hardware Control (like LED)

void TaskonCore0(void* pvParameters) {
    for (;;) {
        if (isNewDataComing) {
            A2D();
            LEDControl();
        }
    }
}

void A2D() {
    // 加速度 轉 位移
}

void LEDControl() {
    // 提供 按壓是否太快或太慢的資訊給使用者
}

/*******************************************************************************/

void setup() {
    // Serial Configuration
    Serial.begin(BAUDRATE);

    //
    accelerometer.begin();

    //
    initData();

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

void loop() {
    Filter();
    SendData();
}

/*******************************************************************************/