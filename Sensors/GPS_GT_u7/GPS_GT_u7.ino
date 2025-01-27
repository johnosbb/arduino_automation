#include <SoftwareSerial.h>

SoftwareSerial GPSSerial(10, 11); //(RX, TX) check correct pin assignments.

//--------------------------------------
//NOTE: you should upload the code first and then connect to GPS module
//Wiring guide
//GT-U7 GPS    ----  Arduino UNO R3 / OPEN-SMART UNO
// VCC         ----   5V
// GND         ----   GND
// RXD         ----   NOT CONNECT
// TXD         ----   RX (Pin 10) for Soft Serial
// PPS         ----   NOT CONNECT
//--------------------------------------
#define GPRMC_TERM "$GPRMC,"		//Defines the instruction to be parsed, and it contains positioning and time information

char nmeaSentence[68];
String latitude;		
String longitude;		
String lndSpeed;		
String gpsTime;			//time of prime meridian
String gmtTime;		//time in London

void setup()	
{
  Serial.begin(9600);		
  GPSSerial.begin(9600);	
  delay(10);
}

void loop()		//
{

  // For one second we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < 1000;)	//Keep searching GPS information in one second
  {
    while (GPSSerial.available())	//The serial port gets the data and starts parsing
    {
      char c = GPSSerial.read();	

      switch(c)					
      {
      case '$':					//$ is the beginning of a frame of data
        GPSSerial.readBytesUntil('*', nmeaSentence, 67);		
        //Serial.println(nmeaSentence);
        latitude = parseGprmcLat(nmeaSentence);	
        longitude = parseGprmcLon(nmeaSentence);
        lndSpeed = parseGprmcSpeed(nmeaSentence);
        gpsTime = parseGprmcTime(nmeaSentence);


        if(latitude > "")		//If it is not null, print out the value
        {
          Serial.println("------------------------------------");
          Serial.println("latitude: " + latitude);
        }

        if(longitude > "")		//If it is not null, print out the value
        {
          Serial.println("longitude: " + longitude);
        }  

        if(lndSpeed > "")		//If it is not null, print out the value
        {
          Serial.println("Speed (knots): " + lndSpeed);
        }

        if(gpsTime > "")		//If it is not null, print out the value
        {
          Serial.println("gpsTime: " + gpsTime);
          gmtTime = getGMTTime(gpsTime);	//Calculate GMT time 
          Serial.println("GMT Time: " + gmtTime);        
        }		
      }
    }
  }
}

String getGMTTime(String s)
{
  int hour = s.substring(0, 2).toInt();   // Extract hours
  int minute = s.substring(2, 4).toInt(); // Extract minutes
  int second = s.substring(4, 6).toInt(); // Extract seconds

  // Format hours, minutes, and seconds with leading zeros if needed
  String formattedTime = (hour < 10 ? "0" : "") + String(hour) + ":" +
                         (minute < 10 ? "0" : "") + String(minute) + ":" +
                         (second < 10 ? "0" : "") + String(second);

  return formattedTime;
}

//Parse GPRMC NMEA sentence data from String
//String must be GPRMC or no data will be parsed
//Return Latitude
String parseGprmcLat(String s)
{
  int pLoc = 0; //paramater location pointer
  int lEndLoc = 0; //lat parameter end location
  int dEndLoc = 0; //direction parameter end location
  String lat;
  /*make sure that we are parsing the GPRMC string. 
   Found that setting s.substring(0,5) == "GPRMC" caused a FALSE.
   There seemed to be a 0x0D and 0x00 character at the end. */
  if(s.substring(0,4) == "GPRM")
  {
    //Serial.println(s);
    for(int i = 0; i < 5; i++)
    {
      if(i < 3) 
      {
        pLoc = s.indexOf(',', pLoc+1);
        /*Serial.print("i < 3, pLoc: ");
         Serial.print(pLoc);
         Serial.print(", ");
         Serial.println(i);*/
      }
      if(i == 3)
      {
        lEndLoc = s.indexOf(',', pLoc+1);
        lat = s.substring(pLoc+1, lEndLoc);
        /*Serial.print("i = 3, pLoc: ");
         Serial.println(pLoc);
         Serial.print("lEndLoc: ");
         Serial.println(lEndLoc);*/
      }
      else
      {
        dEndLoc = s.indexOf(',', lEndLoc+1);
        lat = lat + " " + s.substring(lEndLoc+1, dEndLoc);
        /*Serial.print("i = 4, lEndLoc: ");
         Serial.println(lEndLoc);
         Serial.print("dEndLoc: ");
         Serial.println(dEndLoc);*/
      }
    }
    return lat; 
  }
  //}
  //}
}

//Parse GPRMC NMEA sentence data from String
//String must be GPRMC or no data will be parsed
//Return Longitude
String parseGprmcLon(String s)
{
  int pLoc = 0; //paramater location pointer
  int lEndLoc = 0; //lat parameter end location
  int dEndLoc = 0; //direction parameter end location
  String lon;

  /*make sure that we are parsing the GPRMC string. 
   Found that setting s.substring(0,5) == "GPRMC" caused a FALSE.
   There seemed to be a 0x0D and 0x00 character at the end. */
  if(s.substring(0,4) == "GPRM")
  {
    //Serial.println(s);
    for(int i = 0; i < 7; i++)
    {
      if(i < 5) 
      {
        pLoc = s.indexOf(',', pLoc+1);
        /*Serial.print("i < 3, pLoc: ");
         Serial.print(pLoc);
         Serial.print(", ");
         Serial.println(i);*/
      }
      if(i == 5)
      {
        lEndLoc = s.indexOf(',', pLoc+1);
        lon = s.substring(pLoc+1, lEndLoc);
        /*Serial.print("i = 3, pLoc: ");
         Serial.println(pLoc);
         Serial.print("lEndLoc: ");
         Serial.println(lEndLoc);*/
      }
      else
      {
        dEndLoc = s.indexOf(',', lEndLoc+1);
        lon = lon + " " + s.substring(lEndLoc+1, dEndLoc);
        /*Serial.print("i = 4, lEndLoc: ");
         Serial.println(lEndLoc);
         Serial.print("dEndLoc: ");
         Serial.println(dEndLoc);*/
      }
    }
    return lon; 
  }
}

//Parse GPRMC NMEA sentence data from String
//String must be GPRMC or no data will be parsed
//Return Longitude
String parseGprmcSpeed(String s)
{
  int pLoc = 0; //paramater location pointer
  int lEndLoc = 0; //lat parameter end location
  int dEndLoc = 0; //direction parameter end location
  String lndSpeed;

  /*make sure that we are parsing the GPRMC string. 
   Found that setting s.substring(0,5) == "GPRMC" caused a FALSE.
   There seemed to be a 0x0D and 0x00 character at the end. */
  if(s.substring(0,4) == "GPRM")
  {
    //Serial.println(s);
    for(int i = 0; i < 8; i++)
    {
      if(i < 7) 
      {
        pLoc = s.indexOf(',', pLoc+1);
        /*Serial.print("i < 8, pLoc: ");
         Serial.print(pLoc);
         Serial.print(", ");
         Serial.println(i);*/
      }
      else
      {
        lEndLoc = s.indexOf(',', pLoc+1);
        lndSpeed = s.substring(pLoc+1, lEndLoc);
        /*Serial.print("i = 8, pLoc: ");
         Serial.println(pLoc);
         Serial.print("lEndLoc: ");
         Serial.println(lEndLoc);*/
      }
    }
    return lndSpeed; 
  }
}


//Parse GPRMC NMEA sentence data from String
//String must be GPRMC or no data will be parsed
//Return Longitude
String parseGprmcTime(String s)
{
  int pLoc = 0; //paramater location pointer
  int lEndLoc = 0; //lat parameter end location
  int dEndLoc = 0; //direction parameter end location
  String gpsTime;

  /*make sure that we are parsing the GPRMC string. 
   Found that setting s.substring(0,5) == "GPRMC" caused a FALSE.
   There seemed to be a 0x0D and 0x00 character at the end. */
  if(s.substring(0,4) == "GPRM")
  {
    //Serial.println(s);
    for(int i = 0; i < 2; i++)
    {
      if(i < 1) 
      {
        pLoc = s.indexOf(',', pLoc+1);
        /*Serial.print("i < 8, pLoc: ");
         Serial.print(pLoc);
         Serial.print(", ");
         Serial.println(i);*/
      }
      else
      {
        lEndLoc = s.indexOf(',', pLoc+1);
        gpsTime = s.substring(pLoc+1, lEndLoc);
        /*Serial.print("i = 8, pLoc: ");
         Serial.println(pLoc);
         Serial.print("lEndLoc: ");
         Serial.println(lEndLoc);*/
      }
    }
    return gpsTime; 
  }
}

// Turn char[] array into String object
String charToString(char *c)
{

  String val = "";

  for(int i = 0; i <= sizeof(c); i++) 
  {
    val = val + c[i];
  }

  return val;
}


