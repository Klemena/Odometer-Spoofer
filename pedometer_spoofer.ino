#include "Arduino.h"
#include "Joystick.h"
#include "Servo.h"
#include <TM1637Display.h>

#define CLK 11
#define DIO 12
#define JOYSTICK_PIN_SW  2
#define JOYSTICK_PIN_VRX  A3
#define JOYSTICK_PIN_VRY  A4
#define SERVOMD_PIN_SIG 3

uint8_t dist[] = {0x47 , 0x6d, 0x06, 0x73};
uint8_t numbers[] = {0x3f, 0x30, 0x5b, 0x79,0x74, 0x6d, 0x6f, 0x38, 0x7f, 0x7c};
uint8_t data[4];
uint8_t blank[] = {0x00, 0x00,0x00,0x00};
int currentDisplayValue;
int distanceToStepCoefficent = 90;

//TESTING VALUES
int joystickDownThreshold = 510;
int joystickUpThreshold = 130;
int servoDegreeMotion = 60;
int sleepTimer = 30; //Time for the machine to sleep in seconds to ensure voltage regulation occurs
int breakTimer = 100; //Amount of time in seconds until voltage regulation is needed

const uint8_t SEG_DONE[] = {
  SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,           // d
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,   // O
  SEG_C | SEG_E | SEG_G,                           // n
  SEG_A | SEG_D | SEG_E | SEG_F | SEG_G            // E
  };

Joystick joystick(JOYSTICK_PIN_VRX,JOYSTICK_PIN_VRY,JOYSTICK_PIN_SW);
Servo servo;
TM1637Display display(CLK, DIO);

void setup() {
  // put your setup code here, to run once:
  servo.attach(3);
  servo.write(0);
  display.setBrightness(0x0f);
  currentDisplayValue = -1;
  
}

void loop() {

  if(currentDisplayValue == -1)
  {
    display.setSegments(dist);
  }
  else
  {
     updateCurrentDisplayValue();
  }

  if(joystick.getY() > joystickDownThreshold)
  {
    currentDisplayValue--;
    if(currentDisplayValue < 0)
    {
      currentDisplayValue = 9999;
    }
  }

  if(joystick.getY() < joystickUpThreshold)
  {
    currentDisplayValue++;
  }

  if(joystick.getSW() == 1 && currentDisplayValue != -1)
  {
    runDistance(currentDisplayValue);
    currentDisplayValue = -1;
  }
 
}

//CURRENT: 3, 2, 1, 0
void setValue(int digit, int value)
{
  data[digit] = numbers[value];
  display.setSegments(data);
}

void runDistance(int distance)
{
  int steps = distance * distanceToStepCoefficent;
  int placeholder = currentDisplayValue;
  
  for(int i = steps; i > -1; i--)
  {
    if(i % breakTimer == 0)
    {
        display.setSegments(blank);
        delay(1000 * sleepTimer);
    }
    currentDisplayValue = i;
    updateCurrentDisplayValue();
    servo.write(0);
    delay(500);
    servo.write(servoDegreeMotion);
    delay(500); 

    if(joystick.getSW() == 1)
    {
      pause();
    }
  }
  currentDisplayValue = placeholder;
}

void pause()
{
    delay(1000);
    while(true)
    {
      if(joystick.getSW() == 1)
      {
        break;
      }
      else
      {
        display.setSegments(blank);
        delay(1000);
        updateCurrentDisplayValue();
        }
    }
}

void updateCurrentDisplayValue()
{
  if(currentDisplayValue > 9999)
  {
    currentDisplayValue = 0;
  }
  int placeholder = currentDisplayValue;
  for(int i = 0; i < 4; i++)
  {
    setValue(i, placeholder % 10);
    placeholder = placeholder / 10;
  }
}
