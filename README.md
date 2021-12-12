# NEO Pixel Clock
NEO pixel clock

#### Status: Completed - In Service ####

#### Devices: ESP8266, WS2812, Logic Level Converter ####

### Description: ###
Based on the neo pixels chips WS2812 5050, I purchased 8m of LED strips with 144LEDs/meter.

![alt text](images/Default_C.jpg "Default Colors")
![alt text](images/Rainbow_F.jpg "Vertical Colors")
![alt text](images/Rainbow_H.jpg "Vertical Colors")

#### Features: ####

- Displays actual time (hh:mm:ss)
- Internal RTC to keep time and updates from NTP once every minute

#### Credits
- Neo7segment.h:   [UnexpectedMaker](https://github.com/UnexpectedMaker/Neo7Segment)

The initial library neo7segment.h has been modified by me to adjust the number of pixels per segment and has now been merged to master. The original library gave the possibility to adjust the number of digits, but not the quantity of pixels per segment. Since I wanted a very large clock display (8 pixels per segment) and 4 pixels digits for the outdoor temperature/humidity and for the pool water temperature, I had to modify the library to suite my needs.
