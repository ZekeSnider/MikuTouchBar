#include <stdint.h>
#include "TouchScreen.h"

#define PRESSURE_THRESHOLD 300
#define SCREEN_WIDTH 1000
#define INVALID_TOUCH -50

//XP, YP, XM, YM
//Port definitions for where the touch screens are plugged in
byte pins[1][4] = {
  { 9, A2, A3, 8}
};

//Array to hold the touch screen objects
TouchScreen* touchScreens[2];
int latestReading[2] = {INVALID_TOUCH, INVALID_TOUCH};

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate

void setup(void) {
  int index = 0;
  
  //Instantiate all the touchscreens.
  for (auto& pin : pins) {
    touchScreens[index] = new TouchScreen(pin[0], pin[1], pin[2], pin[3], PRESSURE_THRESHOLD);
    index++;
  }
  
  Serial.begin(9600);
}

void loop(void) {
  int index = 0;
  //Get a reading from each touch screen
  for (auto& ts : touchScreens) {
    // a point object holds x y and z coordinates
    TSPoint thisPoint = ts->getPoint();

    //Check if the reading exceeds the pressure threshold
    if (thisPoint.z > ts->pressureThreshhold) {
       TransformPoint(thisPoint, index);
       Serial.print("X = "); Serial.print(thisPoint.x);
       Serial.print("\tY = "); Serial.print(thisPoint.y);
       Serial.print("\tPressure = "); Serial.println(thisPoint.z);

       for (auto& aLatest : latestReading) {
          if (thisPoint.x - aLatest <= 10) {
            Serial.print("swiping right!!");
          }
          else if (latestReading[index] - aLatest <= 10) {
            Serial.print("swiping left!!");
          }  
       }

       //Store this as the latest. 
       latestReading[index] = thisPoint.x;
    }
    //If the screen did not reach pressure treshold, write invalid value.
    else {
      latestReading[index] = INVALID_TOUCH;
    }

    index++;
  }
  
  delay(100);
}

void TransformPoint(TSPoint& inPoint, int index) {
   inPoint.x += SCREEN_WIDTH * index;
   return;
}

