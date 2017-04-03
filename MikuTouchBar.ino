#include <stdint.h>
#include "TouchScreen.h"
#define TOUCHSCREEN_COUNT 2
#define PRESSURE_THRESHOLD 1
#define READ_DELAY 50
#define MIN_SWIPE
#define SCREEN_WIDTH 865
#define INVALID_TOUCH -5000

int R1Pin = 5;
int R2Pin = 3;
int L1Pin = 4;
int L2Pin = 2;

//XP, YP, XM, YM
//Port definitions for where the touch screens are plugged in
byte pins[TOUCHSCREEN_COUNT][4] = {
  { 7, A4, A5, 6},
  { 9, A2, A3, 8}
};

enum class Direction { left, right };

//Array to hold the touch screen objects
TouchScreen* touchScreens[TOUCHSCREEN_COUNT];
int latestReading[TOUCHSCREEN_COUNT] = {INVALID_TOUCH, INVALID_TOUCH};


// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate

void setup(void) {
  int index = 0;

  pinMode(R1Pin, OUTPUT);
  pinMode(R2Pin, OUTPUT);
  pinMode(L1Pin, OUTPUT);
  pinMode(L2Pin, OUTPUT);
  
  //Instantiate all the touchscreens.
  for (auto& pin : pins) {
    touchScreens[index] = new TouchScreen(pin[0], pin[1], pin[2], pin[3], 300);
    index++;
  }

  Serial.begin(9600);
}

void loop(void) {
  int thisReading[TOUCHSCREEN_COUNT] = {INVALID_TOUCH, INVALID_TOUCH};
  int index = 0;
  int RButton = 0;
  int LButton = 0;
  
  //Get a reading from each touch screen
  for (auto& ts : touchScreens) {
    // a point object holds x y and z coordinates
    TSPoint thisPoint = ts->getPoint();

    //Check if the reading exceeds the pressure threshold
    if (thisPoint.z > PRESSURE_THRESHOLD) {
      Serial.print("\n----------\n");
      TransformPoint(thisPoint, index);
      int count = 0;
      for (auto& aLatest : latestReading) {
        if (aLatest != INVALID_TOUCH) {
          /*
          Serial.print("this reading: ");
          Serial.print(thisPoint.x);
          Serial.print("\n");*/
          
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
          /*
          Serial.print(" alatest = ");
          Serial.print(aLatest);
          Serial.print(" diff = ");
          Serial.print(diff);*/

          if (diff <= 180 && diff > 3) {
            if (swipeDirection == Direction::right) {
              Serial.print(" swiping right");
              RButton++;
            }
            else {
              Serial.print(" swiping left");
              LButton++;
            }
          }
        }
        count++;
      }

      //Store this as the latest.
      thisReading[index] = thisPoint.x;
    }
    //If the screen did not reach pressure treshold, write invalid value.
    else {
      thisReading[index] = INVALID_TOUCH;
    }
    
    index++;
  }
  
  SetPins(RButton, Direction::right);
  SetPins(LButton, Direction::left);

  //Copy thisReading array to the latestReading for next time.
  for (int i = 0; i < TOUCHSCREEN_COUNT; i++) {
    latestReading[i] = thisReading[i];
  }
  delay(READ_DELAY);
}


void SetPins(int& buttonNum, Direction swipeDirection) {
  if (buttonNum >= 2) {
    if (swipeDirection == Direction::right) {
      digitalWrite(R2Pin, LOW);
      digitalWrite(R1Pin, LOW);
    }
    else {
      digitalWrite(L2Pin, LOW);
      digitalWrite(L1Pin, LOW);
    }
  }
  else if (buttonNum == 1) {
    if (swipeDirection == Direction::right) {
      digitalWrite(R2Pin, HIGH);
      digitalWrite(R1Pin, LOW);
    }
    else {
      digitalWrite(L2Pin, HIGH);
      digitalWrite(L1Pin, LOW);
    }
  }
  else {
    if (swipeDirection == Direction::right) {
      digitalWrite(R2Pin, HIGH);
      digitalWrite(R1Pin, HIGH);
    }
    else {
      digitalWrite(L2Pin, HIGH);
      digitalWrite(L1Pin, HIGH);
    }
  }
}

void TransformPoint(TSPoint& inPoint, int index) {
  //inPoint.x -= 70;
  inPoint.x += SCREEN_WIDTH * index;
  return;
}

