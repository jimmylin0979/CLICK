// /*
//  * VARIABLES
//  * count: variable to hold count of rr peaks detected in 10 seconds
//  * flag: variable that prevents multiple rr peak detections in a single heatbeat
//  * hr: HeartRate (initialised to 72)
//  * hrv: Heart Rate variability (takes 10-15 seconds to stabilise)
//  * instance1: instance when heart beat first time
//  * interval: interval between second beat and first beat
//  * timer: variable to hold the time after which hr is calculated
//  * value: raw sensor value of output pin
//  */
// long instance1 = 0, timer;
// double hrv = 0, hr = 72, interval = 0;
// int value = 0, count = 0;
// bool flag = 0;
// #define shutdown_pin 10
// #define threshold 150       // to identify R peak
// #define timer_value 10000   // 10 seconds timer to calculate hr

// void setup() {
//     Serial.begin(9600);
//     pinMode(10, INPUT);   // Setup for leads off detection LO +
//     pinMode(11, INPUT);   // Setup for leads off detection LO -
// }
// void loop() {
//     if ((digitalRead(8) == 1) || (digitalRead(9) == 1)) {
//         Serial.println(0);
//         digitalWrite(shutdown_pin, LOW);   //standby mode
//         instance1 = millis();
//         timer = millis();
//     } else {
//         digitalWrite(shutdown_pin, HIGH);   //normal mode
//         value = analogRead(A0);
//         value = map(value, 250, 400, 0, 100);   //to flatten the ecg values a bit
//         Serial.println(value);
//         if ((value > threshold) && (!flag)) {
//             count++;
//             //Serial.println("in");
//             flag = 1;
//             interval = millis() - instance1;   //RR interval
//             instance1 = millis();
//             //Serial.println(interval);

//         } else if ((value < threshold)) {
//             flag = 0;
//         }
//         if ((millis() - timer) > 30000) {
//             hr = count * 2;
//             Serial.println("30s");
//             timer = millis();
//             count = 0;
//         }
//         hrv = hr / 60 - interval / 1000000;
//         //Serial.print(hr);
//         //Serial.print(",");
//         //Serial.print(hrv);
//         //Serial.print(",");
//         //Serial.println(value);
//         delay(1);
//     }
// }
