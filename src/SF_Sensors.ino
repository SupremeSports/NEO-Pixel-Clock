// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- SENSOR FUNCTIONS ------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initSensors()
{
  Sprintln("Init Sensors...");
  
  //INPUTS
  pinMode(boardLedPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  //OUTPUTS
  pinMode(boardLedPin, OUTPUT);                    // Initialize the LED_BUILTIN pin as an output
}

void readSensors(bool all)
{
  readButtonPush();

  if (!all)
    return;

  //TODO
}

//OUTPUTS
void writeOutputs()
{  
  return;
}

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ ANALOG SENSOR -------------------------------------------
// ----------------------------------------------------------------------------------------------------
void readAnalogSensors()
{
  local_delay(1);
  return;
}

uint16_t readAn(uint8_t pinToRead)
{
  analogRead(pinToRead); //Dump first reading
  local_delay(10);
  return(analogRead(pinToRead));
}

float readAnAvg(uint8_t pinToRead, uint16_t iterations)
{
  uint16_t samples[iterations];
  analogRead(pinToRead); //Dump first reading
  for (int i=0; i<iterations; i++)
  {
    samples[i] = analogRead(pinToRead);
    local_delay(10);
  }

  float average = 0;
  for (int i=0; i<iterations; i++)
     average += samples[i];

  average /= iterations;
  
  return average; //Return average of values
}

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ DIGITAL SENSOR ------------------------------------------
// ----------------------------------------------------------------------------------------------------
void readButtonPush()
{
  bool buttonPressed = !digitalRead(buttonPin);
  if (buttonPressed)
  {
    if (!buttonActive)
    {
      buttonActive = true;
      lastButtonPress = millis();
    }

    int pressedTime = millis() - lastButtonPress;

    if (pressedTime <= shortPressTime && pressedTime >= 50)
      shortPressActive = true;

    if (pressedTime > medPressTime)
      medPressActive = true;

    if (pressedTime > longPressTime)
      longPressActive = true;
  }
  else
  {
    if (longPressActive)
    {
      forceOFF = !forceOFF;
      resetColor = true;

      if (!forceOFF)
      {
        forceON = true;
        forceTimeON = millis();
        resetColor = true;
        newStart = true;
        displayFeature = 0;
        Sprintln("Long press - Power ON");
      }
      else
      {
        forceON = false;
        resetColor = true;
        Sprintln("Long press - Power OFF");
      }
    }
    else if (medPressActive)
    {
      forceON = true;
      forceTimeON = millis();
      resetColor = true;
      
      Sprintln("Med press");
    }
    else if (shortPressActive)
    {
      Sprintln("short press");
      changeColor = true;
    }

    shortPressActive = false;
    medPressActive = false;
    longPressActive = false;
    buttonActive = false;
  }

  //At 1:59 AM, reset all modes
  if (Hour == 1 && Minute == 59)
  {
    colorAutoSwitch = false;
    forceON = false;
    forceON = false;
    resetColor = true;
    
    if (colorAutoSwitch)
      displayFeature = 0;
  }
}



// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- Utility functions -----------------------------------------
// ----------------------------------------------------------------------------------------------------
void setBoardLED(bool newState)
{
  if (!enableBoardLED)
    newState = boardLedPinRevert;

  if (enableBoardLED)
    digitalWrite(boardLedPin, newState);
}

void flashBoardLed(int delayFlash, int qtyFlash)
{
  for (int i=0; i < qtyFlash; i++)
  {
    setBoardLED(!boardLedPinRevert);
    local_delay(delayFlash);
    setBoardLED(boardLedPinRevert);
    local_delay(delayFlash);
  }
}

//Short flash every 5 seconds when everything is ok
void flashEvery5sec()
{
  if (networkActive)
    flashBoardLed(2, 1);
}

float kelvinToFahrenheit(float kelvin)
{
  float celsius = kelvin - 273.15;
  return (celsiusToFahrenheit(celsius));
}

float kelvinToCelsius(float kelvin)
{
  return (kelvin - 273.15);
}

float celsiusToFahrenheit(float celsius)
{
  return (((9.0/5.0) * celsius) + 32);
}

float fahrenheitToCelsius(float fahrenheit)
{
  return ((5.0/9.0) * (fahrenheit - 32));
}

//Custom map function required with ESPNTPClient.h library
long Map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}