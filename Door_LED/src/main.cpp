#include <Arduino.h>

//Pinout
const int reedPin = 32;
const int mosfetPin = 22;
const int switchPin = 14;

//Global variables
int lastDuty = 255;
bool previousSwitchState = LOW;
bool previousReedState = LOW;

void setup()
{
  Serial.begin(115200);  //[for on screen outputs]

  //Assign pins using internal pulldown resistor (to avoid floating state)
  pinMode(reedPin, INPUT_PULLDOWN);
  pinMode(switchPin, INPUT_PULLDOWN);
  
  //PWM setup (5000hz to avoid flickering)
  ledcSetup(0, 5000, 8);
  ledcAttachPin(mosfetPin, 0);
  ledcWrite(0, 255);
}

void loop()
{
  //Input pins definition
  bool currentSwitchState = digitalRead(switchPin);
  bool reedState = digitalRead(reedPin);

  //Push button switch edge detection to change luminosity setting  
  if((currentSwitchState != previousSwitchState) && (currentSwitchState == 1))
  {
    if(lastDuty < 250)
    {
      lastDuty += (255*0.25);
      if(reedState != LOW)      //[important for instant feedback -> without it, lumnosity 
                                // only changes when the dimming loops run vvv]
      {
        ledcWrite(0,lastDuty);
      }
    } 
    else
    {
      lastDuty = 0;             //[]
      if(reedState != LOW)      //[important for instant feedback -> without it, lumnosity 
                                // only changes when the dimming loops run vvv]
      {
        ledcWrite(0,lastDuty);
      }
    }
    delay(200); //[adds delay to debounce push button switch]

    //Outputs lumonosity (when connected to a computer)
    Serial.print("switch: "); Serial.print(currentSwitchState);
    Serial.print("\n lastduty: "); Serial.print(lastDuty); Serial.print("\n\n");
  }

  previousSwitchState = currentSwitchState;

  //Reed switch edge detection to dim in  
  if(reedState && (previousReedState == LOW))
  {
    for(int i = 0; i <= lastDuty; i++)
    {
      ledcWrite(0, i);
      delay(10);
    }
    previousReedState = HIGH;
  }

  //Reed switch edge detection to dim out
  if(!reedState && (previousReedState == HIGH))
  {
    for(int i = lastDuty; i >= 0; i--)
    {
      ledcWrite(0, i);
      delay(10); 
    }
    previousReedState = LOW;
  }
}
