/*
Copyright 2018 Jean Gauthier (gauthier_j100@hotmail.com). All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are
permitted provided that the following conditions are met :

1. Redistributions of source code must retain the above copyright notice, this list of
conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, this list
of conditions and the following disclaimer in the documentation and / or other materials
provided with the distribution.

THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ''AS IS'' AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.IN NO EVENT SHALL <COPYRIGHT HOLDER> OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
	SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
	ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT(INCLUDING
		NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
	ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	The views and conclusions contained in the software and documentation are those of the
	authors and should not be interpreted as representing official policies, either expressed
	or implied
*/

/*
  Name:		    DO2 Control NeoPixel Clock
  Created:	  2019/02/28
  Author:     gauthier_j100@hotmail.com / SupremeSports
  GitHub:     https://github.com/SupremeSports/
*/

/*
  ArduinoOTA            1.0.5
  ESP8266WiFi           1.0
  NTPclient             1.3.2 // IMPORTANT - v1.x.x or older, v2 and v3 fails the clock
  Board                 Generic ESP8266 Module
  Board firmware        ESP8266 2.7.4
*/

const char* version               = "v:5.2.1";
const char* date                  = "2021/12/14";

// ----------------------------------------------------------------------------------------------------
// ----------------------------------------- ESP/WIFI DEFINES -----------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <ESP8266WiFi.h>

#define ESP8266                     7
//ADC_MODE(ADC_VCC);                              //Read Vcc on ADC input

//Network settings - PLEASE, define those values in a config.h file
#include "config.h"

//USING DHCP, so no static IP

bool networkActive                = false;        //WiFi connectivity status
int rssiPercent                   = 0;            //WiFi signal strength in percent
int rssi                          = 0;            //WiFi signal strength in dBm

int wifiServerIndex               = 0;

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- WDT DEFINES --------------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <Ticker.h>
Ticker lwdTicker;

#define LWD_TIMEOUT                 5000          //WDT Value (ms)

unsigned long lwdTime             = 0;

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- OTA DEFINES --------------------------------------------
// ----------------------------------------------------------------------------------------------------
#define ENABLE_OTA

#ifdef ENABLE_OTA
  #include <ESP8266mDNS.h>
  #include <WiFiUdp.h>
  #include <ArduinoOTA.h>
#endif

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ DEBUG DEFINES -------------------------------------------
// ----------------------------------------------------------------------------------------------------
//#define DEBUG

#ifdef DEBUG
  #define Sprintln(a) (Serial.println(a))
  #define Sprint(a) (Serial.print(a))
#else
  #define Sprintln(a)
  #define Sprint(a)
#endif

#define BAUDRATE                    115200

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ OTHER DEFINES -------------------------------------------
// ----------------------------------------------------------------------------------------------------
//Used Pins
const uint8_t boardLedPin         = 2;            //Pin 1 on ESP-01, pin 2 on ESP-12E
const bool boardLedPinRevert      = true;         //If true, LED is on when output is low
const bool enableBoardLED         = true;         //If true, LED will flash to indicate status

//Variables
#define initValue                   99            //Initialization value to insure values updates

long lastSecond                   = 0;

bool newStart                     = false;        //New start detection
bool ONS_1s                       = false;
bool ONS_5s                       = false;
bool ONS_10s                      = false;
bool ONS_1m                       = false;

bool PRE_5s                       = false;        //Comes on for the second before ONS_5s comes on

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ EEPROM DEFINES ------------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <EEPROM.h>

#define EEPROM_SIZE                 64

#define EEPROM_BYTES                17

int address_ENB                   = 0;  //Days enabled (bitwise) / Bit 7=Always_ON
int address_DEF                   = 1;  //Default sequence (radio buttons)
int address_BRI                   = 2;  //Default brightness
int address_SUN_S                 = 3;
int address_SUN_E                 = 4;
int address_MON_S                 = 5;
int address_MON_E                 = 6;
int address_TUE_S                 = 7;
int address_TUE_E                 = 8;
int address_WED_S                 = 9;
int address_WED_E                 = 10;
int address_THU_S                 = 11;
int address_THU_E                 = 12;
int address_FRI_S                 = 13;
int address_FRI_E                 = 14;
int address_SAT_S                 = 15;
int address_SAT_E                 = 16;

// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- SPECIFIC DEFINES ------------------------------------------
// ----------------------------------------------------------------------------------------------------
//const uint8_t sda               = 0;            //I2C Data pin
//const uint8_t scl               = 2;            //I2C Clock pin

// ---------------------------------- NEOPIXEL SEVEN SEGMENT DEFINES ----------------------------------
#include <Neo7Segment.h>

//Used Pins
#define timePin                     14

//Displays setup
#define timeDigits                  4
#define timePixels                  4
#define timePixDp                   2

// Initalise the displays
Neo7Segment timeDisplay(timeDigits, timePixels, timePixDp, timePin);

uint8_t defaultBrightness         = 0;
uint8_t brightness                = defaultBrightness;

//All possible alphabet characters that can be displayed on a 7 segment
const String alphabet[26]         = {"A","B","C","D","E","F","G","H","I","J"," ","L"," ","N","O","P","Q","R","S","T","U"," "," ","X","Y","Z"};

bool resetColor                   = false;          //Reset colors
bool changeColor                  = false;          //Change display color 
bool initDisplays                 = true;           //Initialize displays

//Rainbow indexes and variables
unsigned long nextRainbow         = 0;
unsigned long nextSwitch          = 0;
byte rainbowIndex                 = 0;
uint8_t displayFeature            = 96;             //Initialize displays
uint8_t loopIndex                 = 0;

uint8_t defaultFeature            = 0;

// ------------------------------------------ TIME DISPLAY --------------------------------------------
bool activeDisplay                = true;

typedef struct
{
  bool enable;
  float startTime;
  float stopTime;
} dow_struct;

dow_struct schedule[7];

bool alwaysON                     = false;

String timeString;                                  //Time display string

// -------------------------------------- ANALOG SENSORS DEFINES --------------------------------------
//TODO

// --------------------------------------------- BUTTONS ----------------------------------------------
unsigned long forceTimeON         = 0;              //Timestamp of force ON request event
bool forceON                      = false;          //Display is forced ON no matter the time of day
bool forceOFF                     = false;          //Display is forced OFF no matter the time of day

unsigned long lastButtonPress     = 0;              //Timestamp of button pressed event

bool buttonActive                 = false;          //Button is currently pressed (used to one shot timer)
bool shortPressActive             = false;          //Button has reached short press delay
bool medPressActive               = false;          //Button has reached medium press delay
bool longPressActive              = false;          //Button has reached long press delay

bool colorAutoSwitch              = false;          //Displays are in auto color switch mode (scrolls between modes)

#define shortPressTime              250
#define medPressTime                2000
#define longPressTime               5000
#define extraTime                   3600000

#define buttonPin                   0

// ----------------------------------------------------------------------------------------------------
// --------------------------------------- WEBSERVER CONTROL ------------------------------------------
// ----------------------------------------------------------------------------------------------------
WiFiServer server(80);

String DOW_names[7]               = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
String DOW_tag[7]                 = {"sun_en", "mon_en", "tue_en", "wed_en", "thu_en", "fri_en", "sat_en"};
String DOW_tagStart[7]            = {"sun_start", "mon_start", "tue_start", "wed_start", "thu_start", "fri_start", "sat_start"};
String DOW_tagStop[7]             = {"sun_stop", "mon_stop", "tue_stop", "wed_stop", "thu_stop", "fri_stop", "sat_stop"};

String DOW_tagBrightness          = "bright";
String DOW_tagColor               = "color";
String DOW_tagAlwaysON            = "always";

String DOW_tagColorNames[8]       = {"Blue", "Green", "Red", "Rainbow Cycle", "Color", "SPOON", "Vertical Rainbow", "Horizontal Rainbow"};

const char* SAVE_BUTTON           = "save";
const char* CANCEL_BUTTON         = "cancel";
const char* RESYNCH_BUTTON        = "resynch";
const char* RESTART_BUTTON        = "restart";

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ TIME CONTROL --------------------------------------------
// ----------------------------------------------------------------------------------------------------
#include <TimeLib.h>              //TimeLib library is needed https://github.com/PaulStoffregen/Time
#include <NtpClientLib.h>         //Include NtpClient library header

ntpClient *NTP;

int maxRetryNTP                   = 5;

//Variables
uint16_t Year                     = initValue;
uint8_t Month                     = initValue;
uint8_t Day                       = initValue;
uint8_t Hour                      = initValue;
uint8_t Minute                    = initValue;
uint8_t Second                    = initValue;
uint8_t DOW                       = initValue;
bool DST                          = false;

//Up since...
uint16_t YearUp                   = initValue;
uint8_t MonthUp                   = initValue;
uint8_t DayUp                     = initValue;
uint8_t HourUp                    = initValue;
uint8_t MinuteUp                  = initValue;
uint8_t SecondUp                  = initValue;
bool DSTUp                        = false;

uint8_t prevHour                  = initValue;
uint8_t prevMinute                = initValue;
uint8_t prevSecond                = initValue;

bool localTimeValid               = false;          //Detect that local time is valid
bool upSinceONS                   = false;

// ----------------------------------------------------------------------------------------------------
// --------------------------------------------- SETUP ------------------------------------------------
// ----------------------------------------------------------------------------------------------------
void setup()
{
  #ifdef DEBUG
    Serial.begin(BAUDRATE);
    while (!Serial);
  #endif

  initWDT();

  initSensors();
  initEEPROM();

  initWifi();
  initOTA();
  initNtp();
  
  initDisplay();

  initWebServer();

  local_delay(5);                                //Wait for all data to be ready

  //Initialize data
  newStart = true;
  loopIndex = 0;

  Sprintln("Init completed...");
}

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ MAIN LOOP -----------------------------------------------
// ----------------------------------------------------------------------------------------------------
void loop()
{
  runEveryScan();

  //runEverySecond();

  runEvery5seconds();

  //runEvery10seconds();
  
  //runEveryMinute();

  newStart = false;                               //First scan bit
}

void runEveryScan()
{
  wdtReset();
  
  runOTA();

  networkActive = checkNetwork();

  readSensors(false);                           //Read sensors (buttons, etc.)
  updateTime();                                 //Update current time
  updateTimeString();                           //Display current time
  autoColorChanging();                          //Control colors and string output
  runWebServer();

  prevSecond = Second;
  prevMinute = Minute;
  prevHour = Hour;

  local_delay(5);
}

void runEverySecond()
{
  if (!ONS_1s)
    return;

  unsigned long loopTime = millis();

  ONS_1s = false;

  //TODO

  loopTime = millis() - loopTime;
  Sprint("Process time: ");
  Sprint(loopTime);
  Sprintln("ms");
}

void runEvery5seconds()
{
  if (!ONS_5s)
    return;

  ONS_5s = false;
  PRE_5s = false;
    
  Sprintln("5 seconds");

  flashEvery5sec();
  
  local_delay(10);
}

void runEvery10seconds()
{
  if (!ONS_10s)
    return;

  ONS_10s = false;
    
  Sprintln("10 seconds");
  
  //TODO
}

void runEveryMinute()
{
  if (!ONS_1m)
    return;

  ONS_1m = false;

  unsigned long loopTime = millis();

  //TODO

  loopTime = millis() - loopTime;
  Sprint("Process time (minute): ");
  Sprint(loopTime);
  Sprintln("ms");
}
