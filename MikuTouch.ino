#include <stdint.h>
#include "TouchScreen.h"

#define TOUCHSCREEN_COUNT 1
#define PRESSURE_THRESHOLD -1
#define READ_DELAY 50

#define MIN_SWIPE

#define SCREEN_WIDTH 865
#define INVALID_TOUCH -50

int R1Pin = 4;
int L1Pin = 3;
int R2Pin = 4;
int L2Pin = 5;

//XP, YP, XM, YM
//Port definitions for where the touch screens are plugged in
byte pins[TOUCHSCREEN_COUNT][4] = {
  { 7, A4, A5, 6}
};

enum class Direction { left, right };

//Array to hold the touch screen objects
TouchScreen* touchScreens[TOUCHSCREEN_COUNT];
int latestReading[TOUCHSCREEN_COUNT] = {INVALID_TOUCH};


// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate

void setup(void) {
  int index = 0;

  pinMode(R1Pin, OUTPUT);
  digitalWrite(R1Pin, HIGH);
  
  //Instantiate all the touchscreens.
  for (auto& pin : pins) {
    touchScreens[index] = new TouchScreen(pin[0], pin[1], pin[2], pin[3], 650);
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
    if (thisPoint.z > PRESSURE_THRESHOLD) {
      TransformPoint(thisPoint, index);

      for (auto& aLatest : latestReading) {
        if (aLatest != INVALID_TOUCH) {
          Direction swipeDirection;
          int diff;
          if (thisPoint.x > aLatest) {
            diff = thisPoint.x - aLatest;
            swipeDirection = Direction::right;
          }
          else {
            diff = aLatest - thisPoint.x;
            swipeDirection = Direction::left;
          }

          Serial.print("thisPoint = ");
          Serial.print(thisPoint.x);
          Serial.print(" alatest = ");
          Serial.print(aLatest);
          Serial.print(" diff = ");
          Serial.print(diff);

          if (diff <= 180 && diff > 3) {
            if (swipeDirection == Direction::right) {
              Serial.print(" swiping right");
              digitalWrite(R1Pin, LOW);
            }
            else {
              Serial.print(" swiping left");
              //digitalWrite(L1Pin, LOW);
            }
          }
          else {
            //digitalWrite(R1Pin, HIGH);
          }

          Serial.print("\n");
        }
      }

      //Store this as the latest.
      latestReading[index] = thisPoint.x;
      index++;
    }
    //If the screen did not reach pressure treshold, write invalid value.
    else {
      //digitalWrite(R1Pin, HIGH);
      latestReading[index] = INVALID_TOUCH;
    }

    index++;
  }

  delay(READ_DELAY);
}

void TransformPoint(TSPoint& inPoint, int index) {
  inPoint.x -= 70;
  inPoint.x += SCREEN_WIDTH * index;
  return;
}

