// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ INIT WEBSERVER ------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initWebServer()
{
  // Start the server
  server.begin();
  Sprintln("Server started...");
 
  // Print the IP address
  Sprint("Use this URL to connect: ");
  Sprint("http://");
  Sprint(WiFi.localIP());
  Sprintln("/");
  Sprint("Use this URL to connect: ");
  Sprint("http://");
  Sprint(hostName);
  Sprintln("/");
}

// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- RUN WEBSERVER ------------------------------------------
// ----------------------------------------------------------------------------------------------------
void runWebServer()
{
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
   
  // Wait until the client sends some data
  Sprintln("new client");
  while(!client.available()){
    local_delay(1);
  }

  wdtReset();
   
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Sprintln(request);
  client.flush();
   
  // Match the request
  int value = 0;

  if (request.indexOf(SAVE_BUTTON) != -1)
    submit(request);
  else if (request.indexOf(CANCEL_BUTTON) != -1)
    cancel();
  else if (request.indexOf(RESTART_BUTTON) != -1)
  {
    client.print("<HEAD>");
    client.print("<meta http-equiv=\"refresh\" content=\"0;url=/\">");
    client.print("</head>");
    timeDisplay.SetBrightness(map(defaultBrightness,0,100,0,255));
    timeDisplay.DisplayTextColor("BOOT", timeDisplay.Color(255, 0, 0));
    client.stop();
    while(1);
  }

  wdtReset();
 
  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");

  client.println("<h1>NEOPIXEL CLOCK MANAGEMENT</h1>");

  client.print("<div style='position:absolute; text-indent: 0px;'><b>Weekly Schedule</b></div>");
  client.print("<div style='position:absolute; text-indent: 130px;'><b>Enable</b></div>");
  client.print("<div style='position:absolute; text-indent: 207px;'><b>Start</b></div>");
  client.print("<div style='position:absolute; text-indent: 275px;'><b>Stop</b></div><br>");
  
  client.print("<form method=get>");
  
  for (int i=0; i<7; i++)
  {
    //Enable day
    client.print(DOW_names[i]);
    client.print("<input type='checkbox' name='");
    client.print(DOW_tag[i]);
    client.print("' style='position:absolute; left:151px;'");
    client.print(schedule[i].enable ? "checked>" : ">");
    
    //Start time
    client.print("<input type='number' inputmode='decimal' step='0.5' min='0' max '24' name='");
    client.print(DOW_tagStart[i]);
    client.print("' value=");
    client.print(schedule[i].startTime, 1);
    client.print(" style='position:absolute; left:210px; width:40px;'>");

    //Stop time
    client.print("<input type='number' inputmode='decimal' step='0.5' min='0' max '24' name='");
    client.print(DOW_tagStop[i]);
    client.print("' value=");
    client.print(schedule[i].stopTime, 1);
    client.print(" style='position:absolute; left:275px; width:40px;'>");

    //change line
    client.println("<br><br>");
    wdtReset();
  }

  wdtReset();

  //Always ON
  client.print("Always On");
  client.print("<input type='checkbox' name='");
  client.print(DOW_tagAlwaysON);
  client.print("' style='position:absolute; left:150px;'");
  client.print(alwaysON ? "checked>" : ">");
  //change line
  client.println("<br><br><br>");

  //Brightness
  client.print("<b>Brightness</b>");
  client.print("<input type='range' step='5' min='5' max '100' name='");
  client.print(DOW_tagBrightness);
  client.print("' value=");
  client.print(defaultBrightness);
  client.print(" style='position:absolute; left:150px; width:171px;' oninput='showBrightnessValue(this.value)'><br>");
  client.print("Active: ");
  client.print(defaultBrightness);
  client.print("%<br>");
  client.print("New: ");
  client.println("<span id='range1'>0</span>"); 
  client.println("<script type='text/javascript'>\r\nfunction showBrightnessValue(newValue)\r\n{\r\ndocument.getElementById('range1').innerHTML=newValue;\r\n}\r\n</script>\r\n");
  client.println("<script type=!text/javascript!></script\r\n>");
  client.print("%<br>");
  //change line
  client.print("<br><br>");

  //Default color radio buttons
  client.print("<b>Default Color</b><br>");
  for (int i=0; i<8; i++)
  {
    client.print(DOW_tagColorNames[i]);
    client.print("<input type='radio' name='");
    client.print(DOW_tagColor);
    client.print("' value=");
    client.print(i);
    client.print(" style='position:absolute; left:151px;'");
    client.print(defaultFeature==i ? "checked>" : ">");
    
    //change line
    client.println("<br>");
  }

  client.println("<br><br>");

  //Adjust Year
  client.print("<b>EEPROM Year</b>");
  client.print("<input type='number' inputmode='decimal' step='1' min='0' max '24' name='");
  client.print(DOW_tagYear);
  client.print("' value=");
  client.print(currentYear+2000);
  client.print(" style='position:absolute; left:151px; width:50px;'>");

  client.println("<br><br><br>");

  //Save/Cancel buttons
  client.print("<input type=submit value=Save name=save'> <input type=submit value=Cancel name=cancel> <input type=submit value=Reboot name=restart></a>");

  client.println("</form>");

  client.println("<br>");

  //Ownership
  client.print("<div style='position:absolute; text-indent: 0px; font-size:12px;'><b>Jean Gauthier ing./P.Eng. (C) 2021</b></div><br>");
  client.print("<div style='position:absolute; text-indent: 0px; font-size:12px;'>Licensed under <a href = 'https://github.com/SupremeSports/NEO-Pixel-Clock/blob/main/LICENSE' target='_blank' rel='noopener noreferrer'>Creative Commons Zero v1.0 Universal</a></div><br>");
  client.print("<div style='position:absolute; text-indent: 0px; font-size:12px;'>Visit <a href = 'https://github.com/SupremeSports/NEO-Pixel-Clock' target='_blank' rel='noopener noreferrer'>GitHub</a> code repository</div><br>");
  client.print("<br>");

  //NTP Server
  client.print("<div style='position:absolute; text-indent: 0px; font-size:12px;'><b>NTP Server:</b></div>");
  client.print("<div style='position:absolute; text-indent: 151px; font-size:12px;'>");
  client.print(timeServer[wifiServerIndex]);
  client.print("</div><br>");
  
  //System Date
  client.print("<div style='position:absolute; text-indent: 0px; font-size:12px;'><b>System Date:</b></div>");
  client.print("<div style='position:absolute; text-indent: 151px; font-size:12px;'>");
  client.print(Year);
  client.print("/");
  client.print(Month);
  client.print("/");
  client.print(Day);
  client.print("</div><br>");

  //Version
  client.print("<div style='position:absolute; text-indent: 0px; font-size:12px;'><b>Version:</b></div>");
  client.print("<div style='position:absolute; text-indent: 151px; font-size:12px;'>");
  client.print(version);
  client.print("</div><br>");

  //Version Date
  client.print("<div style='position:absolute; text-indent: 0px; font-size:12px;'><b>Last Update:</b></div>");
  client.print("<div style='position:absolute; text-indent: 151px; font-size:12px;'>");
  client.print(date);
  client.print("</div><br>");
  
  client.println("</html>");

  wdtReset();
  local_delay(1);
  Sprintln("");
  client.stop();
}

void submit(String message)
{
  int pos1 = 0;
  int pos2 = 0;
  String newString = "";
  
  for (int i=0; i<7; i++)
  {
    //DOW enable
    schedule[i].enable = message.indexOf(DOW_tag[i]) != -1;

    //DOW start time
    pos1 = message.indexOf(DOW_tagStart[i]) + DOW_tagStart[i].length();
    pos2 = message.indexOf('&', pos1);
    newString = message.substring(pos1 + 1, pos2);
    schedule[i].startTime = newString.toFloat();

    //DOW stop time
    pos1 = message.indexOf(DOW_tagStop[i]) + DOW_tagStop[i].length();
    pos2 = message.indexOf('&', pos1);
    newString = message.substring(pos1 + 1, pos2);
    schedule[i].stopTime = newString.toFloat();
  }

  //Always ON
  alwaysON = message.indexOf(DOW_tagAlwaysON) != -1;

  //Brightness
  pos1 = message.indexOf(DOW_tagBrightness) + DOW_tagBrightness.length();
  pos2 = message.indexOf('&', pos1);
  newString = message.substring(pos1 + 1, pos2);
  defaultBrightness = newString.toInt();

  //Default color
  pos1 = message.indexOf(DOW_tagColor) + DOW_tagColor.length();
  pos2 = message.indexOf('&', pos1);
  newString = message.substring(pos1 + 1, pos2);
  defaultFeature = newString.toInt();
  rainbowIndex = 0;
  displayFeature = defaultFeature;

  //Adjust Year
  pos1 = message.indexOf(DOW_tagYear) + DOW_tagYear.length();
  pos2 = message.indexOf('&', pos1);
  newString = message.substring(pos1 + 1, pos2);
  uint16_t yearAdjust = newString.toInt();

  if (yearAdjust-2000 != currentYear)
  {
    Year = yearAdjust;
    currentYear = yearAdjust-2000;
    localTimeValid = false;
  }

  wdtReset();
  writeEEPROM();
}

void cancel()
{
  readEEPROM();
}
