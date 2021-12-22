# NEO Pixel Clock

#### Status: Completed - In Service ####

#### Devices: ESP8266, WS2812, Logic Level Converter ####

### Description: ###
Based on the neo pixels chips WS2812 5050, I purchased 8m of LED strips with 144LEDs/meter.

Colors examples:

![alt text](images/Default_C.jpg "Default Colors")
![alt text](images/Rainbow_V.jpg "Vertical Colors")
![alt text](images/Rainbow_H.jpg "Horizontal Colors")

Web server available on `http://yourHostname/` (defined in config.h) or with the IP address:

![alt text](images/webserver.jpg "Webserver")

#### Features: ####

- Displays actual time (hh:mm) and ":" are flashing every second
- Internal RTC to keep time and updates from NTP once every hour or if time is lost (reboot)
- Get time from NTP server, has a custom function to calculate DST based on your local rules
- Webserver can be found at local IP (reported on serial bus) or by typing http://neoclock/
- Physical button on GPIO0 for programming, but can also change color (must not be pressed on power up)

#### Credits
- Neo7segment.h:   [UnexpectedMaker](https://github.com/UnexpectedMaker/Neo7Segment)
The initial library neo7segment.h has been modified by me to adjust the number of pixels per segment and has now been merged to master. The original library gave the possibility to adjust the number of digits, but not the quantity of pixels per segment. Since I wanted a very large clock display [on another project](https://github.com/SupremeSports/HA-Domotic/tree/master/Devices/Pool%20Clock) (8 pixels per segment) and 4 pixels digits for the outdoor temperature/humidity and for the pool water temperature, I had to modify the library to suite my needs.

- NtpClientLib.h:     [gmag11](https://github.com/gmag11/NtpClient/commits/1.3.2)
The library has to be exactly 1.3.2 otherwise the time request function resets the clock