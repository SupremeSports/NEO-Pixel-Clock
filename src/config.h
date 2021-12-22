// ----------------------------------------------------------------------------------------------------
// ------------------------------------------- NETWORK SETUP ------------------------------------------
// ----------------------------------------------------------------------------------------------------
//You must put your own settings in here
//YOU ALSO MIGHT ADJUST DST RULES in file SF_Time in function "bool isDST(int day, int month, int dow)"

const char* hostName                  = "yourHostname";

const int   ssid_qty                  = 2;                      //You can set as many as you want, WiFi setup loop will try all of them until one works
const char* ssid[ssid_qty]            = {"yourSSID", "yourSSID"};
const char* password[ssid_qty]        = {"yourPassword", "yourPassword"};
const char* OTApassword               = "yourPassword";

const char* timeServer[ssid_qty]      = {"yourNTPservIP or name", "pool.ntp.org"}; //If you want the default, set name to "pool.ntp.org"

const int timeZone                    = yourTimeZone;           //Timezone when on standard time (e.g.: -5 for US & Canada Estearn time)
const int timeZoneDST                 = yourTimeZoneDST;        //Timezone when on daylight saving time (if none, set it to the same as timeZone) (e.g.: -4 for US & Canada Estearn time)
