/*
 * Project neopixelstrips
 * Description: For controlling neopixel strips via a Particle photon.
 * Author: Dustin Webb
 * Date: 2017-10-30
 */
 #include <math.h>
 #include <neopixel.h>
 #ifdef __AVR__
   #include <avr/power.h>
 #endif

 #define pixelsPIN 3

 int LED = D7;

 Adafruit_NeoPixel strip = Adafruit_NeoPixel(144, pixelsPIN); //, NEO_GRB + NEO_KHZ800);

 uint8_t mode = 16;
 uint8_t num_modes = 19;

 uint8_t wheelIdx;

 unsigned long LEDDeltaT = 50;
 unsigned long prevLEDDeltaT = LEDDeltaT;

 uint8_t currentPixelIdx = 0;

 uint8_t theaterChaseMode = 0;
 uint8_t theaterChaseIdx = 0;

 bool use_rainbow = false;
 uint32_t currentColor = 0;
 uint8_t currentR = 255;
 uint8_t currentG = 255;
 uint8_t currentB = 255;

 bool success = false;

 int setMode(String new_mode_str);
 int setDelay(String new_delay_str);
 int setR(String new_r_str);
 int setG(String new_g_str);
 int setB(String new_b_str);
 int toggleRainbow(String junk);

 void setup()
 {
     pinMode(LED, OUTPUT);

     Particle.variable("mode", mode);
     Particle.variable("delay", LEDDeltaT);

     success = Particle.function("mode", setMode);
     success = Particle.function("delay", setDelay);
     success = Particle.function("R", setR);
     success = Particle.function("G", setG);
     success = Particle.function("B", setB);
     success = Particle.function("rainbow", toggleRainbow);

     strip.setBrightness(75);
     strip.begin();
     currentColor = strip.Color(currentR, currentG, currentB);
     strip.show(); // Initialize all pixels to 'off'
 }

 void loop () {
     ++wheelIdx;
     wheelIdx % 256;

     if (mode == 0) {
         // NOOP
     } else  if (mode == 1) {
         rainbowCycle();
     } else if (mode == 2) {
         theaterChaseRainbow();
     } else if (mode == 3) {
         theaterChase(currentColor);
     } else if (mode == 4) {
         colorWipe(currentColor);
     } else if (mode == 5) {
         currentColor = strip.Color(0, 0, 255);
         rainbowSparkle(currentColor, 0.334);
     } else if (mode == 6) {
         sparkle(currentColor, 0.334);
     } else if (mode == 7) {
         int g = random(115);
         currentColor = strip.Color(255, g, 0);
         theaterChase(currentColor);
     } else if (mode == 8) {
         int g = random(115);
         currentColor = strip.Color(255, g, 0);
         colorWipe(currentColor);
     } else if (mode == 9) {
         int g = random(115);
         currentColor = strip.Color(255, g, 0);
         sparkle(currentColor, 0.334);
     } else if (mode == 10) {
         int g = random(115);
         currentColor = strip.Color(255, g, 0);
         orangeSparkle(currentColor, 0.334);
     }
     delay(LEDDeltaT);
 }

 // turn off all pixels
 void turnOff() {
     uint32_t c = strip.Color(0, 0, 0);
     for(uint16_t pIdx=0; pIdx<strip.numPixels(); pIdx++) {
       strip.setPixelColor(pIdx, c);
     }
     strip.show();
 }

 // Cloud functions must return int and take one String
 int setMode(String new_mode_str) {
     if (new_mode_str == "off") {
         prevLEDDeltaT = LEDDeltaT;
         LEDDeltaT = 1000;
         turnOff();
         mode = 0;
     } else {
         LEDDeltaT = prevLEDDeltaT;
         if (new_mode_str == "cycle") {
             mode = 1;
         } else if (new_mode_str == "theater") {
             if (use_rainbow == true) {
                 mode = 2;
             } else {
                 mode = 3;
             }
         } else if (new_mode_str == "wipe") {
             mode = 4;
         } else if (new_mode_str == "sparkle") {
             if (use_rainbow == true) {
                 mode = 5;
             } else {
                 mode = 6;
             }
         } else {
             int new_mode = atoi(new_mode_str);
             mode = new_mode;
         }
     }
     return mode;
 }

 int setR(String new_r_str) {
     int new_r = atoi(new_r_str);
     currentR = new_r;
     currentColor = strip.Color(currentR, currentG, currentB);
     return currentR;
 }

 int setG(String new_g_str) {
     int new_g = atoi(new_g_str);
     currentG = new_g;
     currentColor = strip.Color(currentR, currentG, currentB);
     return currentG;
 }

 int setB(String new_b_str) {
     int new_b = atoi(new_b_str);
     currentB = new_b;
     currentColor = strip.Color(currentR, currentG, currentB);
     return currentB;
 }

 int toggleRainbow(String junk) {
     if (use_rainbow == true) {
         use_rainbow = false;
     } else {
         use_rainbow = true;
     }
     if (use_rainbow == true) {
         return 1;
     } else {
         return 0;
     }
 }


 int setDelay(String new_delay_str) {
     int new_delay = atoi(new_delay_str);
     LEDDeltaT = new_delay;
     return 0;
 }

 // randomly light pixels
 void sparkle(uint32_t c, float prob) {
   for(uint16_t pIdx=0; pIdx<strip.numPixels(); pIdx++) {
     float randNum = random(strip.numPixels()) / (float)strip.numPixels();
     if (randNum < prob) {
       strip.setPixelColor(pIdx, c);
       strip.show();
     } else {
       strip.setPixelColor(pIdx, 0);
     }
   }
   strip.show();
 }

 // randomly light pixels
 void rainbowSparkle(uint32_t c, float prob) {
   for(uint16_t pIdx=0; pIdx<strip.numPixels(); pIdx++) {
     float randNum = random(strip.numPixels()) / (float)strip.numPixels();
     if (randNum < prob) {
       strip.setPixelColor(pIdx, Wheel(((pIdx * 256 / strip.numPixels()) + wheelIdx) & 255));
       strip.show();
     } else {
       strip.setPixelColor(pIdx, 0);
     }
   }
   strip.show();
 }

 // randomly light pixels with shade of orange
 void orangeSparkle(uint32_t c, float prob) {
   for(uint16_t pIdx=0; pIdx<strip.numPixels(); pIdx++) {
       //float randNum = random(strip.numPixels()) / (float)strip.numPixels();
       int g = random(150)/150.0;
       //g = -log(-g+1)/0.5; // Sample from exponential distribution with heavier tail (i.e. \lamba = 0.5) http://www.colorado.edu/economics/morey/6818/randomdraws.pdf
       currentColor = strip.Color(255, 50, 0);
       strip.setPixelColor(pIdx, currentColor);
       //strip.show();
   }
   strip.show();
 }

 void wipePixels() {
   for (uint8_t idx = 0; idx < strip.numPixels(); ++idx) {
     strip.setPixelColor(idx, 0);
   }
   strip.show();
 }

 // Fill the dots one after the other with a color
 void colorWipe(uint32_t c) {
   if (currentPixelIdx == 0) {
     wipePixels();
   }
   strip.setPixelColor(currentPixelIdx, c);
   strip.show();
   ++currentPixelIdx;
   currentPixelIdx %= strip.numPixels();
 }

 void rainbow(uint8_t wait) {
   uint16_t i, j;

   for(j=0; j<256; j++) {
     for(i=0; i<strip.numPixels(); i++) {
       strip.setPixelColor(i, Wheel((i+j) & 255));
     }
     strip.show();
     delay(wait);
   }
 }

 // Slightly different, this makes the rainbow equally distributed throughout
 void rainbowCycle() {
   for(uint16_t i=0; i< strip.numPixels(); i++) {
     strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + wheelIdx) & 255));
   }
   strip.show();
 }

 //Theatre-style crawling lights.
 void theaterChase(uint32_t c) {
     if (theaterChaseMode == 0) {
       theaterChaseOn(c);
       theaterChaseMode = 1;
     } else {
       theaterChaseOff();
       theaterChaseMode = 0;
       ++theaterChaseIdx;
     }
     theaterChaseIdx %= 3;
     strip.show();
 }

 //Theatre-style crawling lights.
 void theaterChaseRainbow() {
     if (theaterChaseMode == 0) {
       theaterChaseRainbowOn();
       theaterChaseMode = 1;
     } else {
       theaterChaseOff();
       theaterChaseMode = 0;
       ++theaterChaseIdx;
     }
     theaterChaseIdx %= 3;
     strip.show();
 }

 void theaterChaseOn(uint32_t c) {
   for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
     strip.setPixelColor(i+theaterChaseIdx, c);    //turn every third pixel on
   }
 }

 void theaterChaseRainbowOn() {
   for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
     uint32_t c = Wheel( (i+wheelIdx) % 255);
     strip.setPixelColor(i+theaterChaseIdx, c);    //turn every third pixel on
   }
 }void theaterChaseOff() {
     for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
       strip.setPixelColor(i+theaterChaseIdx, 0);        //turn every third pixel off
     }
 }

 //Theatre-style crawling lights with rainbow effect
 void theaterChaseRainbow(uint8_t wait) {
   for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
     for (int q=0; q < 3; q++) {
       for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
         strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
       }
       strip.show();

       delay(wait);

       for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
         strip.setPixelColor(i+q, 0);        //turn every third pixel off
       }
     }
   }
 }

 // Input a value 0 to 255 to get a color value.
 // The colours are a transition r - g - b - back to r.
 uint32_t Wheel(byte WheelPos) {
   WheelPos = 255 - WheelPos;
   if(WheelPos < 85) {
     return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
   }
   if(WheelPos < 170) {
     WheelPos -= 85;
     return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
   }
   WheelPos -= 170;
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
 }
