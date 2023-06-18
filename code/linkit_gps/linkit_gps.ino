#include <LGSM.h>
#include <LGPS.h>


gpsSentenceInfoStruct info;
char buff[256];
double latitude;
double longitude;
int max_distance=0;

static unsigned char getComma(unsigned char num,const char *str)
{
  unsigned char i,j = 0;
  int len=strlen(str);
  for(i = 0;i < len;i ++)
  {
     if(str[i] == ',')
      j++;
     if(j == num)
      return i + 1; 
  }
  return 0; 
}

static double getDoubleNumber(const char *s)
{
  char buf[10];
  unsigned char i;
  double rev;
  
  i=getComma(1, s);
  i = i - 1;
  strncpy(buf, s, i);
  buf[i] = 0;
  rev=atof(buf);
  return rev; 
}

static double getIntNumber(const char *s)
{
  char buf[10];
  unsigned char i;
  double rev;
  
  i=getComma(1, s);
  i = i - 1;
  strncpy(buf, s, i);
  buf[i] = 0;
  rev=atoi(buf);
  return rev; 
}

void parseGPGGA(const char* GPGGAstr)
{
  /* Refer to http://www.gpsinformation.org/dale/nmea.htm#GGA
   * Sample data: $GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47
   * Where:
   *  GGA          Global Positioning System Fix Data
   *  123519       Fix taken at 12:35:19 UTC
   *  4807.038,N   Latitude 48 deg 07.038' N
   *  01131.000,E  Longitude 11 deg 31.000' E
   *  1            Fix quality: 0 = invalid
   *                            1 = GPS fix (SPS)
   *                            2 = DGPS fix
   *                            3 = PPS fix
   *                            4 = Real Time Kinematic
   *                            5 = Float RTK
   *                            6 = estimated (dead reckoning) (2.3 feature)
   *                            7 = Manual input mode
   *                            8 = Simulation mode
   *  08           Number of satellites being tracked
   *  0.9          Horizontal dilution of position
   *  545.4,M      Altitude, Meters, above mean sea level
   *  46.9,M       Height of geoid (mean sea level) above WGS84
   *                   ellipsoid
   *  (empty field) time in seconds since last DGPS update
   *  (empty field) DGPS station ID number
   *  *47          the checksum data, always begins with *
   */
 
  int tmp, hour, minute, second, num ;
  if(GPGGAstr[0] == '$')
  {
    tmp = getComma(1, GPGGAstr);
    hour     = (GPGGAstr[tmp + 0] - '0') * 10 + (GPGGAstr[tmp + 1] - '0');
    minute   = (GPGGAstr[tmp + 2] - '0') * 10 + (GPGGAstr[tmp + 3] - '0');
    second    = (GPGGAstr[tmp + 4] - '0') * 10 + (GPGGAstr[tmp + 5] - '0');
    
    sprintf(buff, "UTC timer %2d-%2d-%2d", hour, minute, second);
    Serial.println(buff);
    
    tmp = getComma(2, GPGGAstr);
    latitude = getDoubleNumber(&GPGGAstr[tmp]);
    tmp = getComma(4, GPGGAstr);
    longitude = getDoubleNumber(&GPGGAstr[tmp]);
    sprintf(buff, "latitude = %10.4f, longitude = %10.4f", latitude, longitude);
    Serial.println(buff); 
    
    tmp = getComma(7, GPGGAstr);
    num = getIntNumber(&GPGGAstr[tmp]);    
    sprintf(buff, "satellites number = %d", num);
    Serial.println(buff); 
  }
  else
  {
    Serial.println("Not get data"); 
  }
}

/*start setup function*/
void setup() {
    Serial.begin(9600);
    LGPS.powerOn();
    Serial.println("LGPS Power on, and waiting ..."); 
    delay(3000);    
    
    while(!LSMS.ready())
      delay(1000);
      Serial.println("SIM ready for work!");
}
/* start loop function */

void loop()
{
  long int loop=0;
  Serial.println("LGPS loop"); 
  LGPS.getData(&info);
  Serial.println((char*)info.GPGGA); 
  if ((loop%100)==0)
  {
    parseGPGGA((const char*)info.GPGGA);
    delay(2000);
  }


  switch (check_sms()) {
  case 1:
      Serial.print("Ritorna le coordinate attuali:");
      send_coordinates();
    break;
  case 2:
      Serial.print("status dei comandi:");
    break;
  default:
      Serial.print("Default:");
    break;
}
  loop++;
}

int check_sms()
{   
    char buf[20];
    int v;
    int ret=0;
    if (LSMS.available()) // Check if there is new SMS
    {
      Serial.println("There is new message.");
      LSMS.remoteNumber(buf, 20); // display Number part
      Serial.print("Number:");
      Serial.println(buf);
      Serial.print("Content:"); // display Content part
    while(true)
    {
      v = LSMS.read();      
      if(v < 0)
        break;
      //cmd +=v;
      //creare la stringa testo sms
      //inviare questa stringa a parse_sms()
      // parse_sms ritorna il tipo di comando ricevuto

      Serial.print((char)v);
      ret=1;
    }
    
    sprintf(buff, "latitude = %10.4f, longitude = %10.4f", latitude, longitude);
    Serial.println(buff); 
    
    Serial.println();
    LSMS.flush(); // delete message
    }
    delay(1000);
    return ret;
}

/*
parse_sms:
- restituisce il comando:
- GET Coordinate attuali (ritorna le coordinate attuali). restituisce coordinate attuali e OK
- SET PERIODIC TIME X (receive 1 sms ogni x minuti). restituisce OK
- SET DISTANCE y (manda sms quando la distanza dal punto attuale supera i y metri). restituisce OK
- CANCEL ALL cancella tutti i comandi precedenti. restituisce OK
- STATUS: ritorna lo stato di tutti i comandi:
*/

int parse_sms()
{
}

int get_coordinates()
{}

int send_point(String,String)
{}

void send_coordinates()
{
  // get coordinates
  String lat;
  String longit;
  // get delle coordinate correnti
  get_coordinates();
  // invia le coordinate correnti via sms
  send_point(lat,longit);
}

