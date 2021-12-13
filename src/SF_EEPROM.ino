// ----------------------------------------------------------------------------------------------------
// ------------------------------------------ INIT EEPROM ---------------------------------------------
// ----------------------------------------------------------------------------------------------------
void initEEPROM()
{
  Sprintln("Init EEPROM...");
  
  EEPROM.begin(EEPROM_SIZE);
  local_delay(5);
  readEEPROM(); //Wait to let time to display initial show
}

// ----------------------------------------------------------------------------------------------------
// ---------------------------------------- EEPROM FUNCTIONS ------------------------------------------
// ----------------------------------------------------------------------------------------------------
void readEEPROM()
{
  byte values[EEPROM_BYTES];
  for (int i=0; i<EEPROM_BYTES; i++)
    values[i] = EEPROM.read(i);

  defaultFeature = constrain(values[address_DEF],0,7);
  defaultBrightness = constrain(values[address_BRI],5,100);
  alwaysON = bitRead(values[address_ENB], 7)==1;

  int j = 3;
  for (int i=0; i<7; i++)
  {
    schedule[i].enable = bitRead(values[address_ENB], i)==1;
    schedule[i].startTime = constrain(float(values[j])/10.0, 0.0, 24.0);
    j++;
    schedule[i].stopTime = constrain(float(values[j])/10.0, 0.0, 24.0);
    j++;

    wdtReset();
  }

  currentYear = values[address_YEAR];

  Sprintln("Read EEPROM: ");
  for (int i=0; i<EEPROM_BYTES; i++)
    Sprintln(values[i]);
  
}

void writeEEPROM()
{
  byte values[EEPROM_BYTES];

  values[address_DEF] = defaultFeature;
  values[address_BRI] = defaultBrightness;
  bitWrite(values[address_ENB], 7, alwaysON);

  int j = 3;
  for (int i=0; i<7; i++)
  {
    bitWrite(values[address_ENB], i, schedule[i].enable);
    values[j] = (int)(schedule[i].startTime*10.0);
    j++;
    values[j] = (int)(schedule[i].stopTime*10.0);
    j++;

    wdtReset();
  }

  values[address_YEAR] = currentYear;

  Sprintln("Write EEPROM: ");
  for (int i=0; i<EEPROM_BYTES; i++)
  {
    Sprintln(values[i]);
    EEPROM.put(i, values[i]);
  }

  EEPROM.commit();
  
  readEEPROM();
}
