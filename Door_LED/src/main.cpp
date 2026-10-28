#include <Arduino.h>

//Pinout
const int reedPin = 32;
const int mosfetPin = 22;
const int switchPin = 14;

//Global variables
int lastDuty = 255;
bool previousSwitchState = LOW;
bool previousReedState = LOW;
unsigned long lightOnTime;
bool timedOut;

void setup()
{
  Serial.begin(115200);  //[for on screen outputs]

  //Assign pins using internal pulldown resistor (to avoid floating state)
  pinMode(reedPin, INPUT_PULLDOWN);
  pinMode(switchPin, INPUT_PULLDOWN);
  
  //PWM setup (800hz to avoid audible PWM noise and visible light flickering)
  ledcSetup(0, 800, 8);
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
  if((previousReedState == LOW) && reedState)
  {
    lightOnTime = millis();
    for(int i = 0; i <= lastDuty; i++)
    {
      ledcWrite(0, i);
      delay(10);
    }
    previousReedState = HIGH;
    timedOut = false;
  }

  //Reed switch edge detection to dim out
  if((previousReedState == HIGH) && !reedState)
  {
    for(int i = lastDuty; i >= 0; i--)
    {
      ledcWrite(0, i);
      delay(10); 
    }
    previousReedState = LOW;
  }

  //Timeout logic (10min)
  if(reedState && (timedOut == false))
  {
    if((millis() - lightOnTime) >= 600000)
    {
      for(int i = lastDuty; i >= 0; i--)
      {
        ledcWrite(0, i);
        delay(10); 
      }
      timedOut = true;
    }
  }
}