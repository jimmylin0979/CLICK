#include "A2DManager.h"

#include <Arduino.h>
#include <Vector.h>

float A2DManager::integral(IntegralInfo* pre, IntegralInfo* cur) {
    float integralResult = 0;
    float timeInterval = cur->timeStamp - pre->timeStamp;

    if ((cur->value <= 0 && pre->value <= 0) || (cur->value >= 0 && pre->value >= 0)) {
        // both acceleration is negative

        // get the delta Velocity, and plus the delta to preV to get curV
        integralResult = (cur->value + pre->value) * timeInterval / 2;
    } else {
        // one acceleration is positive and the other one is negative
        // get the delta Velocity, and plus the delta to preV to get curV
        float absCurA = abs(cur->value), absPreA = abs(pre->value);
        // QUSETION: 可能溢位
        float preTimeInterval = (absPreA / (absCurA + absPreA)) * timeInterval;
        float curTimeInterval = timeInterval - preTimeInterval;

        integralResult = preTimeInterval * pre->value + curTimeInterval * cur->value;
        integralResult = integralResult / 2;
    }
}