// ----------------------------------------------------------------------------------------------------
// -------------------------------------- CLOCK DISPLAY FUNCTIONS -------------------------------------
// ----------------------------------------------------------------------------------------------------
void initDisplay()
{
  Sprintln("Init NEO Display...");
  
  timeDisplay.Begin(0);   // Start the display with a brightness level of 0%
  
  //Wait until the display is initialised before we try to show anything
  while (!timeDisplay.IsReady())
  {
    flashBoardLed(50, 1);
    local_delay(10);
    wdtReset();
  }

  //Init displays values for initial start
  initDisplays = true;
  displayFeature = 96;
  rainbowIndex = 0;
  nextSwitch = millis();
}

// ----------------------------------------------------------------------------------------------------
// -------------------------------------- TIME DISPLAY FUNCTIONS --------------------------------------
// ----------------------------------------------------------------------------------------------------
//This function gets the text scrolling
void updateTimeString()
{
  //update once per second
  if (Second == prevSecond && !newStart)
    return;

  activeDisplay = (brightness!=0);

  if (localTimeValid)
    timeString = setTimeValues();
  else
    timeString = (Second % 2)==0 ? " . . . ." : "    ";
  setBrightnessLevel();

  if (brightness==0 && activeDisplay)
    timeDisplay.DisplayTextColor("      ", timeDisplay.Color(0, 0, 0));

  Sprintln(timeString);
}

void setBrightnessLevel()
{
  float currentTime = (float)Hour + ((float)Minute/60.0) + ((float)Second/3600.0);
  float todayStartTime = schedule[DOW].startTime;
  float todayStopTime = schedule[DOW].stopTime;
  bool todayON = schedule[DOW].enable;

  if ((currentTime < todayStartTime || currentTime >= todayStopTime || !todayON) && !initDisplays && !alwaysON)
    brightness = 0;
  else
  {
    brightness = Map(defaultBrightness,0,100,0,255);
    forceON = false;
    forceOFF = false;
  }

  if (forceON)
    brightness = Map(defaultBrightness,0,100,0,255);

  if (forceOFF)
    brightness = 0;
  
  timeDisplay.SetBrightness(brightness);
}

// ----------------------------------------------------------------------------------------------------
// ----------------------------------------- STRING GENERATION ----------------------------------------
// ----------------------------------------------------------------------------------------------------
//Send the time to the display
String setTimeValues()
{
  int len = 12;
  //Set displays data
  String digitTime[len];
  digitTime[0]  = (Hour<10) ? " " : String(Hour/10);
  digitTime[2]  = String(Hour%10);
  digitTime[4]  = (Minute<10) ? "0" : String(Minute/10);
  digitTime[6]  = String(Minute%10);
  digitTime[8]  = (Second<10) ? "0" : String(Second/10);
  digitTime[10] = String(Second%10);

  digitTime[1]  = ((Second % 2)==0 && timeDigits>2) ? "." : "";
  digitTime[3]  = ((Second % 2)==0 && timeDigits>2) ? "." : "";
  digitTime[5]  = ((Second % 2)==0 && timeDigits>4) ? "." : "";
  digitTime[7]  = ((Second % 2)==0 && timeDigits>4) ? "." : "";
  digitTime[9]  = ((Second % 2)==0 && timeDigits>6) ? "." : "";
  digitTime[11] = ((Second % 2)==0 && timeDigits>6) ? "." : "";

  String stringOutput;
  for (int i=0; i<len; i++)
    stringOutput += digitTime[i];

  return stringOutput;
}