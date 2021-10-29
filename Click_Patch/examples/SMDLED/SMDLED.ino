// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// Released under the GPLv3 license to match the rest of the
// Adafruit NeoPixel library

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#define duration 4000
#define full_brightness 255
#define NUMPIXELS 7
#define PIN 18
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.

  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  Serial.begin(9600);
}

void loop() {
  breathing_analog();
}

void breathing_analog(){
  pixels.clear(); // Set all pixel colors to 'off'
  
  Serial.println("準備變亮...");
  for(int brightness=0; brightness<=full_brightness; brightness++){
    for(int i=0; i<=NUMPIXELS; i++) { // For each pixel...
      pixels.setPixelColor(i, pixels.Color(brightness, 0, 0));
    } 
    pixels.show();   // Send the updated pixel colors to the hardware.
    delay(duration/full_brightness); // Pause before next pass through loop
    Serial.print(brightness);
    Serial.print(" ");
  }
  Serial.println("");
  Serial.println("準備變暗...");
  for(int brightness=full_brightness; brightness>=0; brightness--){
    for(int i=0; i<=NUMPIXELS; i++) { // For each pixel...
      pixels.setPixelColor(i, pixels.Color(0, brightness, 0));    
    }
    pixels.show();   // Send the updated pixel colors to the hardware.
    delay(duration/full_brightness); // Pause before next pass through loop
    Serial.print(brightness);
    Serial.print(" ");
  }
  
  delay(500);
  Serial.println("");
  Serial.println("完成呼吸...");
}

void breathing_digital(){
  pixels.clear(); // Set all pixel colors to 'off'
  
  Serial.println("準備變亮...");
  int s = duration / 1000;
  for(int _step=0; _step<=s; _step++){
    int brightness = 255 / s * _step;
    for(int i=0; i<=NUMPIXELS; i++) { 
      pixels.setPixelColor(i, pixels.Color(brightness, brightness, brightness));
    } 
    pixels.show();   // Send the updated pixel colors to the hardware.
  if (_step != 0) delay(900); 
    Serial.print(brightness);
    Serial.print(" ");
  }
  Serial.println("");
  Serial.println("準備變暗...");
  for(int _step=s; _step>=0; _step--){
    int brightness = 255 / s * _step;
    for(int i=0; i<=NUMPIXELS; i++) { 
      pixels.setPixelColor(i, pixels.Color(brightness, brightness, brightness));    
    }
    pixels.show();   // Send the updated pixel colors to the hardware.
    if (_step != 4) delay(900); 
    Serial.print(brightness);
    Serial.print(" ");
  }
  
  Serial.println("");
  Serial.println("完成呼吸...");
}
