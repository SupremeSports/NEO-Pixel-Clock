// ----------------------------------------------------------------------------------------------------
// ----------------------------------------- TIME FUNCTIONS -------------------------------------------
// ----------------------------------------------------------------------------------------------------
void updateTime()
{
  ONS_1s = false;
  ONS_5s = false;
  ONS_10s = false;
  ONS_1m = false;

  bool timeOK = true;

  //if ((second() == 0 && minute() == 0 && second() != Second && minute() != Minute) || !localTimeValid)
  //if (minute() != Minute || !localTimeValid)
  if ((minute() == 0 && minute() != Minute) || !localTimeValid)
    timeOK = getNtpTime();

  if (second() == Second && localTimeValid)
    return;

  if (minute() != Minute)
    ONS_1m = true;

  bool updateONS = second() != Second;
  
  Year = year();
  Month = month();
  Day = day();
  Hour = hour();
  Minute = minute();
  Second = second();
  DOW = weekday()-1; // weekday() returns (1-7) ==> (Sun-Sat)

  localTimeValid = (Year>2000 && Year<2100) && timeOK;

  if (!upSinceONS && localTimeValid)
  {
    YearUp = Year;
    MonthUp = Month;
    DayUp = Day;
    HourUp = Hour;
    MinuteUp = Minute;
    SecondUp = Second;
    DSTUp = DST;

    upSinceONS = true;
  }

  if (updateONS)
  {
    ONS_1s = true;
    ONS_5s = Second%5==0;
    ONS_10s = Second%10==0;
  
    PRE_5s = (Second+1)%5==0 || ONS_5s; //Less than a second before next trigger
  }
}

// ----------------------------------------------------------------------------------------------------
// --------------------------------------- NTP SERVER FUNCTIONS ---------------------------------------
// ----------------------------------------------------------------------------------------------------
void initNtp()
{
  // NTP begin with default parameters:
  //   NTP server: pool.ntp.org
  //   TimeZone: UTC
  //   Daylight saving: off

  maxRetryNTP = 5;

  NTP = ntpClient::getInstance(timeServer[wifiServerIndex], 0, false);
  //NTP = ntpClient::getInstance(timeServerDefault, 0, false);
  NTP->begin(); //Starts time synchronization
  local_delay(100);
}

//2085978496 - 07/02/2036 - 06:28:16 ? Default reading, do not accept
bool getNtpTime()
{
  String serverNameStr = NTP->getNtpServerName();
  int len = serverNameStr.length()+1;
  char serverName[len];
  serverNameStr.toCharArray(serverName, len);
  Sprint("Read from ");
  Sprint(serverName);
  Sprintln(" NTP Server");

  time_t DTvalue = ntpClient::getTime();
  if (DTvalue == 0 || DTvalue == 2085978496)
  {
    if (--maxRetryNTP == 0)
    {
      if (!strcmp(timeServerDefault,serverName)==0)
      {
        ntpClient::getInstance(timeServerDefault, 0, false);
        maxRetryNTP = 5;
      }
      else
        while(1);
    }
    
    ESP.getFreeHeap();
    return false;
  }

  ntpClient::getInstance(timeServer[wifiServerIndex], 0, false);
  maxRetryNTP = 5;

  Sprint("UTC Date/Time: ");
  Sprint(DTvalue);
  Sprint(" - ");
  Sprint(NTP->getDateStr(DTvalue));
  Sprint(" - ");
  Sprint(NTP->getTimeStr(DTvalue));
  Sprint(" - ");
  Sprintln(count);

  setTime(DTvalue);
  //setTime(hour(DTvalue), minute(DTvalue), second(DTvalue), day(DTvalue), month(DTvalue), year(DTvalue));

  DST = isDST(day(), month(), weekday()-1);

  int timeAdjust = (DST ? timeZoneDST : timeZone) * 3600;
  adjustTime(timeAdjust);

  ESP.getFreeHeap();

  return true;
}

/*  Those settings are for North America Eastern time
 *  You might need to adjust formulas according to your
 *  location and DST rules.
 */ 
bool isDST(int day, int month, int dow)
{
    //January, February, and December are not DST
    if (month < 3 || month > 11)
      return false;
    //April to October are in DST
    if (month > 3 && month < 11)
      return true;
      
    int previousSunday = day - dow;
    //In march, we are DST if our previous sunday was on or after the 8th.
    if (month == 3)
      return previousSunday >= 8;
      
    //In november we must be before the first sunday to be DST.
    //That means the previous sunday must be before the 1st.
    if (month == 11)
      return previousSunday <= 0;
}