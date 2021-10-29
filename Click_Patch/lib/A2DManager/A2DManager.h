#ifndef __A2DMANAGER_H__
#define __A2DMANAGER_H__

#include <Arduino.h>
#include <Vector.h>

typedef struct IntegralInfo {
    // 積分 對象

    unsigned long timeStamp;   // X : 時間點
    float value;               // Y : 數值
};

class A2DManager {
    private:
    //
    bool isInWave;

    // 一般積分換算
    float integral(IntegralInfo*, IntegralInfo*);
    // 積分完 V波 後處理
    void postProcess();

    //

    public:
    A2DManager(/* args */);
    ~A2DManager();

    float A2DTransfer(IntegralInfo*);
};

#endif