#include <FastLED.h>
#include <NeoPixelBus.h>

const uint16_t len = 17*5*5;
const uint16_t PixelCount = len; // this example assumes 4 pixels, making it smaller will cause a failure
const uint8_t PixelPin = 2;  // make sure to set this to the correct pin, ignored for Esp8266
uint16_t p = 0;

#define colorSaturation 255
unsigned long t;

NeoPixelBus<NeoGrbwFeature, NeoEsp8266Dma800KbpsMethod> strip(PixelCount);

void setup() {
  // put your setup code here, to run once:

    Serial.begin(115200);
    while (!Serial); // wait for serial attach

    Serial.println();
    Serial.println("Initializing...");
    Serial.flush();

    // this resets all the neopixels to an off state
    strip.Begin();
    RgbwColor w = RgbwColor(0, 0, 0, 0);
    
    
    for (uint8_t x = 0; x < 25; x++) {
        uint8_t a;
        uint8_t b = floor(x/5);
        
        if(b%2==0){
          a = x%5;
        }else{
          a = 4-x%a;
        }

        uint8_t c = inoise8(a,b);
        
        if (x % 2 == 0) {
          w = RgbwColor(c, 0, c, 0);
        } else {
          w = RgbwColor(0, c, c, 0);
        }
        
        
        
        for (uint8_t i = 0; i < 17; i++) {
          strip.SetPixelColor(17 * x + i, w);
        }
      }
    

    strip.Show();
}




void loop() {
    if(t < millis() + 100){
      
      RgbwColor w = RgbwColor(0, 0, 0, 0);
      for (uint8_t x = 0; x < 25; x++) {
          uint8_t a;
          uint8_t b = floor(x/5);
          
          if(b%2==0){
            a = x%5;
          }else{
            a = 4-x%a;
          }
  
          uint8_t c = inoise8(a*500,b*500, p*3);
          c = c/2;
          Serial.println(c);
          if (x % 2 == 0) {
            w = RgbwColor(c, 0, c, 0);
          } else {
            w = RgbwColor(0, c, c, 0);
          }
          
          
          
          for (uint8_t i = 0; i < 17; i++) {
            strip.SetPixelColor(17 * x + i, w);
          }
        }
      
  
      strip.Show();
      t = millis();
      p++;
    }
}
