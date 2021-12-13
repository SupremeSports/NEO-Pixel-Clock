// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- COLORS FUNCTIONS ------------------------------------------
// ----------------------------------------------------------------------------------------------------
void changeColorButton()
{
  if (changeColor)
  {
    if (colorAutoSwitch)
      displayFeature = 0;   //If running in auto change mode, return to standard colors when pressing on button
    else
      displayFeature += 1;  //Otherwise, increase feature ID

    rainbowIndex = 0;
    initDisplays = false;
  }

  if (changeColor || resetColor)
  {
    nextRainbow = millis()-1;
    nextSwitch = millis()-1;
    colorAutoSwitch = false;
    changeColor = false;
    loopIndex = 0;
    rainbowIndex = 0;
    resetColor = false;

    rainbowIndex = 0;
    initDisplays = false;
    
    Sprint("Display feature Button: ");
    Sprintln(displayFeature);
  }
}

void autoColorChanging()
{
  changeColorButton();  //Check if a sequence change has been requested

  //Switch what we are showing every 5 seconds
  if (millis() > nextSwitch && colorAutoSwitch)
  {
    nextSwitch = millis() + 5000;
    displayFeature = (displayFeature + 1) % 8; //Modulo number of cases (0 to 7 = 8)
    nextRainbow = millis()-1;

    rainbowIndex = 0;

    Sprint("Display feature AutoChange: ");
    Sprintln(displayFeature);
  }

  if (displayFeature > 2 || initDisplays)
  {
    if (nextRainbow < millis())
    {
      if (initDisplays)
        colorChangingSequences("8.8.8.8.8.8."); //Update color sequence
      else
        colorChangingSequences(timeString);     //Update color sequence
    }
  }
  else
  {
    if (Second != prevSecond)
      colorChangingSequences(timeString); //Update color sequence
  }
}

//Display stuff on the Neo7Segment displays
void colorChangingSequences(String timeStringOutput)
{
  //Wait until the display is initialised before we try to show anything
  if (!timeDisplay.IsReady())
  {
    flashBoardLed(50, 1);
    local_delay(10);
    return;
  }

  if (!activeDisplay)
    return;

  //Sprintln("Output sent");
  
  timeDisplay.ForceUppercase(true);

  uint32_t colorStart;
  uint32_t colorEnd;
  bool newShowUpdate = false;
  
  switch(displayFeature)
  {
    case 0: //Blue
      timeDisplay.DisplayTextColor(timeStringOutput, timeDisplay.Color(0, 0, 255));
      nextRainbow = millis() + 250;
      break;

    case 1: //Green
      timeDisplay.DisplayTextColor(timeStringOutput, timeDisplay.Color(0, 255, 0));
      nextRainbow = millis() + 250;
      break;

    case 2: //Red
      timeDisplay.DisplayTextColor(timeStringOutput, timeDisplay.Color(255, 0, 0));
      nextRainbow = millis() + 250;
      break;
      
    case 3: //Rainbow Cycle
      timeDisplay.DisplayTextColorCycle(timeStringOutput, rainbowIndex);
      nextRainbow = millis() + 10;
      rainbowIndex++;
      break;

    case 4: //Text color
      timeDisplay.DisplayTextColor(timeStringOutput, timeDisplay.Wheel(rainbowIndex & 255));
      //timeDisplay.DisplayTextMarquee(timeStringOutput, 2, timeDisplay.Wheel(rainbowIndex & 255));
      nextRainbow = millis() + 250;
      rainbowIndex+=5;
      break;

    case 5: //SPOON
      loopIndex++;
      if (loopIndex > 1)
        loopIndex = 0;

      timeDisplay.DisplayTextMarquee(timeStringOutput, loopIndex, timeDisplay.Wheel(rainbowIndex & 255));
      nextRainbow = millis() + 250;
      rainbowIndex+=5;
      break;

    case 6: //Vertical Rainbow Cycle
      timeDisplay.DisplayTextVerticalRainbow(timeStringOutput, timeDisplay.Wheel(rainbowIndex & 255), timeDisplay.Wheel((rainbowIndex+50) & 255));
      nextRainbow = millis() + 50;
      rainbowIndex--;
      break;

    case 7: //Horizontal Rainbow Cycle
      timeDisplay.DisplayTextHorizontalRainbow(timeStringOutput, timeDisplay.Wheel(rainbowIndex & 255), timeDisplay.Wheel((rainbowIndex+50) & 255));
      nextRainbow = millis() + 50;
      rainbowIndex--;
      break;

    case 96: //Initialize, show all colors one by one
      timeDisplay.DisplayTextColor(timeStringOutput, timeDisplay.Color(255, 0, 0));
      nextRainbow = millis() + 1000;
      displayFeature++;
      break;

    case 97: //Initialize, show all colors one by one
      timeDisplay.DisplayTextColor(timeStringOutput, timeDisplay.Color(0, 255, 0));
      nextRainbow = millis() + 1000;
      displayFeature++;
      break;

    case 98: //Initialize, show all colors one by one
      timeDisplay.DisplayTextColor(timeStringOutput, timeDisplay.Color(0, 0, 255));
      nextRainbow = millis() + 1000;
      displayFeature++;
      rainbowIndex = 0;
      break;

    case 99: //Initialize, show all colors in rainbow pattern
      timeDisplay.DisplayTextHorizontalRainbow(timeStringOutput, timeDisplay.Wheel(rainbowIndex & 255), timeDisplay.Wheel((rainbowIndex + 50) & 255));
      nextRainbow = millis() + 2;
      rainbowIndex ++;
      if (rainbowIndex >= 255)
      {
        rainbowIndex = 0;
        displayFeature = defaultFeature;
        initDisplays = false;

        timeDisplay.DisplayTextColor("", timeDisplay.Color(0, 0, 0));

        readEEPROM();

        loopIndex = 0;

        Sprintln("Initialization completed...");
      }
      break;
      
    default: //When pressing the button, if getting to last sequence, go into auto change mode until button is repressed
      colorAutoSwitch = true;
      break;
  }
}
