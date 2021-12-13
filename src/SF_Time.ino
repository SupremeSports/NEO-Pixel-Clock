// ----------------------------------------------------------------------------------------------------
// ----------------------------------------- TIME FUNCTIONS -------------------------------------------
// ----------------------------------------------------------------------------------------------------
void updateTime()
{
  ONS_1s = false;
  ONS_5s = false;
  ONS_10s = false;
  ONS_1m = false;

  if ((minute() == 0 && minute() != Minute) || !localTimeValid)
    getNtp();
    
  if (second() == Second && localTimeValid)
    return;

  if (minute() != Minute)
    ONS_1m = true;
  
  Year = year();
  Month = month();
  Day = day();
  Hour = hour();
  Minute = minute();
  Second = second();
  DOW = weekday()-1;

  if (abs((Year-2000) - currentYear) <= 1)
    currentYear = Year-2000;

  localTimeValid = ((Year-2000)==currentYear);

  ONS_1s = true;
  ONS_5s = Second%5==0;
  ONS_10s = Second%10==0;

  PRE_5s = (Second+1)%5==0 || ONS_5s; //Less than a second before next trigger
}

// ----------------------------------------------------------------------------------------------------
// --------------------------------------- NTP SERVER FUNCTIONS ---------------------------------------
// ----------------------------------------------------------------------------------------------------
void initNtp()
{
  //NTP begin with default parameters:
  //NTP server: 'pool.ntp.org' is used with 60 seconds update interval and no offset
  //NTP update: 60 seconds update interval
  //NTP offset: 0
  
  IPAddress serverIP;

  if (serverIP.fromString(timeServer[wifiServerIndex]));
  {
    if (strcmp(timeServer[wifiServerIndex],"pool.ntp.org")==0)
      NTP.setPoolServerName(timeServer[wifiServerIndex]);
    else
      NTP.setPoolServerIP(serverIP);
  }

  NTP.begin();
}

time_t getNtp()
{
  Sprintln("Get NTP...");
  wdtReset();

  NTP.update();

  time_t utcCalc = NTP.getEpochTime(); 
  uint16_t dateTime[6];
  
  dateTime[0] = year(utcCalc);
  dateTime[1] = month(utcCalc);
  dateTime[2] = day(utcCalc);
  dateTime[3] = hour(utcCalc);
  dateTime[4] = minute(utcCalc);
  dateTime[5] = second(utcCalc);

  setTime(dateTime[3], dateTime[4], dateTime[5], dateTime[2], dateTime[1], dateTime[0]);

  bool DST = isDST(day(), month(), weekday()-1);
  int timeAdjust = timeZone*3600;
  if (DST)
    timeAdjust = timeZoneDST*3600;
  
  adjustTime(timeAdjust);
}

bool isDST(int day, int month, int dow)
{
    //January, february, and december are not DST
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
