#define Serial2 DroneSerial

#define USE_OCTOWS2811

#include<OctoWS2811.h>
#include "FastLED.h"  

#include "AnimationManager.h"
#include "Protocol.h"

#define NUM_LEDS_PER_STRIP 9
#define NUM_STRIPS 4
#define ACTUAL_STRIPS 2
#define PORT_SHIFT 2

// Pin layouts on the teensy 3:
// OctoWS2811: 2,14,7,8,6,20,21,5

AnimationManager am;
Protocol proto(am);
  
void setup() {
  DroneSerial.begin(9600);
  Serial.begin(9600);
  
  LEDS.addLeds<OCTOWS2811>(leds, NUM_LEDS_PER_STRIP);
  LEDS.setBrightness(255);
  
  BootAnimation(NULL);
  am.setAnimation(AnimationManager::AnimationType::Idle);
}

void loop() {
  int incomingByte = 0;
  
  if (DroneSerial.available() > 0) {
    incomingByte = DroneSerial.read();
    proto.processByte(incomingByte);
  }
  am.displayCurrentAnimation();  
}

