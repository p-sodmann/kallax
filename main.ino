#include <FastLED.h>
#include <NeoPixelBus.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

WiFiUDP Udp;
unsigned int localUdpPort = 4210;
byte incomingPacket[255];
char  replyPacekt[] = "Hi there! Got the message :-)";

const uint16_t len = 17*5*5;
const uint16_t PixelCount = len; // this example assumes 4 pixels, making it smaller will cause a failure
const uint8_t PixelPin = 2;  // make sure to set this to the correct pin, ignored for Esp8266
uint16_t mil = 0;

uint8_t colorEven[4];
uint8_t colorOdd[4];

uint8_t stage[25][5]; 
uint8_t aniMode = 0;
int lastFrame = 0;

#define colorSaturation 255

NeoPixelBus<NeoGrbwFeature, NeoEsp8266Dma800KbpsMethod> strip(PixelCount);

void setup() {
  // put your setup code here, to run once:
    colorOdd[0] = 0;
    colorOdd[1] = 0;
    colorOdd[2] = 0;
    colorOdd[3] = 255;
    
    colorEven[0] = 0;
    colorEven[1] = 0;
    colorEven[2] = 0;
    colorOdd[2] = 0;
    
    Serial.begin(115200);
    while (!Serial); // wait for serial attach

    Serial.println();
    Serial.println("Initializing...");

    WiFi.hostname("kallax");
    WiFi.begin("Feelan", "meowmeow");
    
    while (WiFi.status() != WL_CONNECTED){
      delay(500);
      Serial.print(".");
    }
    
    Serial.println();
    Serial.println(WiFi.localIP());
    Serial.flush();
    
    // this resets all the neopixels to an off state
    strip.Begin();
    RgbwColor w = RgbwColor(0, 0, 0, 0);
    uint8_t c = 255;
    
    showPattern();
    
    Udp.begin(localUdpPort);
}


void showPattern(){
  RgbwColor w;
  for (uint8_t x = 0; x < 25; x++) {
        if (x % 2 == 0) {
          w = RgbwColor(colorEven[0], colorEven[1], colorEven[2], colorEven[3]);
        } else {
          w = RgbwColor(colorOdd[0],colorOdd[1],colorOdd[2], colorOdd[3]);
        }

        for (uint8_t i = 0; i < 17; i++) {
          strip.SetPixelColor(17 * x + i, w);
        }
      }
    

    strip.Show();
}

void loop() {
  int packetSize = Udp.parsePacket();
  if (packetSize){
    
    int len = Udp.read(incomingPacket, 255);
    
    if (len > 0){
      incomingPacket[len] = 0;
    }

    animationMode = incomingPacket[0];

    switch (animationMode){
      // set a fixed pattern
      case 0:
        colorOdd[0] = incomingPacket[1];
        colorOdd[1] = incomingPacket[2];
        colorOdd[2] = incomingPacket[3];
        colorOdd[3] = incomingPacket[4];
        
        colorEven[0] = incomingPacket[5];
        colorEven[1] = incomingPacket[6];
        colorEven[2] = incomingPacket[7];
        colorEven[3] = incomingPacket[8];
        
        showPattern();
        
        break
        
      case 1:
        // take a value and fade it out
        for(uint8_t i = 0; i < 25; i++){
          if(stage[i][0] < incomingPacket[i*5+0+1]){
            for(uint8_t j = 0; j < 5; j++){            
              stage[i][j] = incomingPacket[i*5+j+1];
            }
          }
        }

      case 2:
        // set constant colors for each box
        for(uint8_t i = 0; i < 25; i++){
          for(uint8_t j = 0; j < 5; j++){            
            stage[i][j] = incomingPacket[i*5+j+1];
          }
            
          strip.SetPixelColor(17 * x + i,
            RgbwColor(
                int(stage[i][1]), 
                int(stage[i][2]), 
                int(stage[i][3]), 
                int(stage[i][4])
            )
          );
        }
        strip.Show();
        break
    }
    
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(replyPacekt);
    Udp.endPacket();
  }
  
  if(aniMode == 1){
    if(millis() >= lastFrame){
      RgbwColor w;  
      lastFrame = millis() +5;
      
      for(uint8_t x = 0; x < 25; x++){
        
        // decrease brightness by 5
        if(stage[x][0] > 0){
          stage[x][0] -= 5;
        }
                 
        for (uint8_t i = 0; i < 17; i++) {
            strip.SetPixelColor(17 * x + i,
              RgbwColor(
                int(stage[x][0] * stage[x][1] / 255), 
                int(stage[x][0] * stage[x][2] / 255), 
                int(stage[x][0] * stage[x][3] / 255), 
                int(stage[x][0] * stage[x][4] / 255)
              )
            );
        }
   
      }
      strip.Show();
    }
  }
}
