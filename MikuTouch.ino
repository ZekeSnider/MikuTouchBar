// Touch screen library with X Y and Z (pressure) readings as well
// as oversampling to avoid 'bouncing'
// This demo code returns raw readings, public domain

#include <stdint.h>
#include "TouchScreen.h"


#define XP 9   // can be a digital pin
#define YP A2  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM 8   // can be a digital pin

#define PRESSURE_THREASHOLD 300

//XP, YP, XM, YM
byte pins[1][4] = {
  { 9, A2, A3, 8}
};

TouchScreen* touchScreens[2];

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(pins[0][0], pins[0][1], pins[0][2], pins[0][3], 300);
//TouchScreen ts2 = TouchScreen(XP, YP, XM, YM, 300);
//TouchScreen ts2 = TouchScreen(XP, YP, XM, YM, 300);

//TouchScreen touchArray [2] = {ts, ts2};

void setup(void) {
  int index = 0;
  for (auto& pin : pins) {
    touchScreens[index] = new TouchScreen(pin[0], pin[1], pin[2], pin[3], PRESSURE_THREASHOLD);
    index++;
  }
  
  Serial.begin(9600);
}

void loop(void) {
  
  for (auto& ts : touchScreens) {
    // a point object holds x y and z coordinates
    TSPoint p = ts->getPoint();
    
    // we have some minimum pressure we consider 'valid'
    // pressure of 0 means no pressing!
    if (p.z > ts->pressureThreshhold) {
       Serial.print("X = "); Serial.print(p.x);
       Serial.print("\tY = "); Serial.print(p.y);
       Serial.print("\tPressure = "); Serial.println(p.z);
    }
  }
  
  delay(100);
}
