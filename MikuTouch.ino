#include <stdint.h>
#include "TouchScreen.h"

#define PRESSURE_THREASHOLD 300
#define SCREEN_WIDTH 1000

//XP, YP, XM, YM
//Port definitions for where the touch screens are plugged in
byte pins[1][4] = {
  { 9, A2, A3, 8}
};

//Array to hold the touch screen objects
TouchScreen* touchScreens[2];

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate

void setup(void) {
  int index = 0;
  
  //Instantiate all the touchscreens.
  for (auto& pin : pins) {
    touchScreens[index] = new TouchScreen(pin[0], pin[1], pin[2], pin[3], PRESSURE_THREASHOLD);
    index++;
  }
  
  Serial.begin(9600);
}

void loop(void) {
  int index = 0;
  for (auto& ts : touchScreens) {
    // a point object holds x y and z coordinates
    TSPoint thisPoint = ts->getPoint();

    // we have some minimum pressure we consider 'valid'
    // pressure of 0 means no pressing!
    if (thisPoint.z > ts->pressureThreshhold) {
       TransformPoint(thisPoint, index);
       Serial.print("X = "); Serial.print(thisPoint.x);
       Serial.print("\tY = "); Serial.print(thisPoint.y);
       Serial.print("\tPressure = "); Serial.println(thisPoint.z);
    }

    index++;
  }
  
  delay(100);
}

void TransformPoint(TSPoint& inPoint, int index) {
   inPoint.x += SCREEN_WIDTH * index;
   return;
}

