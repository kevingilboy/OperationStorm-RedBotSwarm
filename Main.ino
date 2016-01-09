/*
  TEAM SWARM
*/

#include <RedBot.h>
#include <math.h>
#include <Servo.h>
#include "notes.h"

//To customize for each bot, change the constants in the area below
//Do not alter any other code
//////////////////////////////////////////////////////
const int whiteThreshold = 2;//What row: 0,1,2      //
int lVelo = 150;                                    //
int rVelo = 160;                                    //
const int straightDelay = 2700;                     //
const int distMult = 1; //What column: 0, 1, 2      //
const int EastWest = 0; //What way to turn: 1=L,0=R //
const int LturnVelo = 120;                          //
const int RturnVelo = 220;                          //
const int turnTicks = 130;                          //
//////////////////////////////////////////////////////
const int straightTicks = 2000;
// Instantiate the motors.
RedBotMotors motors;

// Instantiate the accelerometer
RedBotAccel accelerometer;

// Instantiate our encoder.
RedBotEncoder encoder = RedBotEncoder(A2, 10); // left, right

// Instantiate the sensors. Sensors can only be created for analog input
//  pins; the accelerometer uses pins A4 and A5. 
RedBotSensor lSen = RedBotSensor(A3);
RedBotSensor mSen = RedBotSensor(A6);
RedBotSensor rSen = RedBotSensor(A7);

// Create an alias for our beeper pin, for ease of use.
#define BEEPER 9

// Create an alias for the onboard pushbutton.
#define BUTTON_PIN 12

// Create an alias for the onboard LED.
#define LED_PIN 13

// Constants for the levels that determine detection of the line.
const int lineLevel = 650;
const int offLevel = 75;

volatile boolean bumped = true;

void setup()
{
  Serial.begin(9600);
  Serial.println("Hello world!");

  // Set up two built-in IO devices- the button and the LED.
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);

  // Play a tone for audible reset detection.
  tone(BEEPER, 2600, 750);
  delay(750);
}


void loop()
{
  bool bumped = true;
  while (bumped)
  {
    if (digitalRead(BUTTON_PIN) == LOW)
    {
      bumped = false;
      encoder.clearEnc(BOTH);
    }
  }
  delay(500);

  int whiteTick = 0;
  int reset = 0;
  while (!bumped)
  {

    // Line following code: turn away from any sensor that is above the line
    //  threshold.

    // Case 1: both white. Drive forward!
    if (lSen.read() < offLevel && rSen.read() < offLevel && mSen.read() > lineLevel)
    {
      motors.drive(110);
      if (mSen.read() > lineLevel)
      {
        reset = 0;
      }
    }
    // Case 2: right sensor is on black. Must recenter.
    if (rSen.read() > lineLevel && lSen.read() < offLevel)
    {
      motors.leftDrive(80);
      motors.rightDrive(-20);
      if (mSen.read() > lineLevel)
      {
        reset = 0;
      }
    }
    // Case 3: left sensor is on black. Must recenter.
    if (lSen.read() > lineLevel && rSen.read() < offLevel)
    {
      motors.rightDrive(80);
      motors.leftDrive(-20);
      if (mSen.read() > lineLevel)
      {
        reset = 0;
      }
    }
    // Case 4: both sensors see dark
    if (lSen.read() > lineLevel && rSen.read() > lineLevel)
    {
      motors.rightDrive(80);
      motors.leftDrive(80);
      if (mSen.read() > lineLevel)
      {
        reset = 0;
      }
    }
    if ( mSen.read() > 50 && mSen.read() < offLevel+50)
    {
      while (reset == 0)
      {
        whiteTick++;
                Serial.print(lSen.read());
                Serial.print("     ");
                Serial.print(mSen.read());
                Serial.print("     ");
                Serial.print(rSen.read());
                Serial.println("");
        motors.brake();
        //delay(250);
        tone(BEEPER, 4000, 100);
        reset = 1; 
      }
      motors.rightDrive(80);
      motors.leftDrive(80);
    }
    if (whiteTick == whiteThreshold)
    {
      //The bot has reached its row number, start encoders and turn bot
      bumped = true;
    }
  }
  motors.brake();
  tone(BEEPER, 4500, 500);
  
  //////////////////////////////////////
  /////////START ENCODERS HERE//////////
  ////////////////////////////////////// 
  
  //FIRST TURN
      turn(EastWest);
      delay(2000);
  //DRIVE STRAIGHT
      //drive();
      motors.rightDrive(rVelo);
      motors.leftDrive(lVelo);
      delay(straightDelay);
      motors.brake();
      delay(2000);
  //SECOND TURN
      if (EastWest == 0)
      {
        turn(1);
      }
      if (EastWest == 1)
      {
        turn(0);
      }
      tone(BEEPER, 3500, 500);

} //Closes void loop


//DRIVING
void drive()
{
  //Declarations
  int baseTicks = straightTicks;
  int ticks = distMult*baseTicks;
  int offset = 5;
  float lDiff = 0, rDiff = 0;
  float lPrev = 0, rPrev = 0;

  encoder.clearEnc(BOTH);
  
  while (encoder.getTicks(RIGHT) < ticks)
  {
    motors.leftDrive(lVelo);
    motors.rightDrive(rVelo);

    lDiff = (encoder.getTicks(LEFT) - lPrev);
    rDiff = (encoder.getTicks(RIGHT) - rPrev);

    lPrev = encoder.getTicks(LEFT);
    rPrev = encoder.getTicks(RIGHT);
    
    if (lDiff > rDiff)
    {
      rVelo = rVelo + (offset);
      Serial.println("Add");
    }
    else if (lDiff < rDiff)
    {
      rVelo = rVelo - (offset);
      Serial.println("Subtract");
    }
    delay(50);
  }
  motors.brake();
}


//TURNING
void turn(int EastWest)
{
  encoder.clearEnc(BOTH);
  int ticks = turnTicks;
  if (EastWest == 1) //LEFT TURN
  {
    motors.rightDrive(RturnVelo);
    while (encoder.getTicks(RIGHT) < ticks)
    {
    }
    motors.brake();       // Stops both motors
  }
  
  if (EastWest == 0) //RIGHT TURN
  {
    motors.leftDrive(LturnVelo);
    while (encoder.getTicks(LEFT) < ticks)
    {
    }
    motors.brake();       // Stops both motors
  }
}
