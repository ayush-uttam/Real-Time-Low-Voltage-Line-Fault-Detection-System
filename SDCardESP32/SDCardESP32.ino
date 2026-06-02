#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>               
#include <TimeLib.h>  
#include <SPI.h>
#include <SD.h>               
 
const int CS = 5;
File myFile;
int SerialNumber = 1; 
const char* ssid = "AYUSH-PC0335";
const char* password = "P!693n57";

WiFiUDP ntpUDP;
 
 //srilankan time zone
 //+5 hr 30 Mins = 5x60x60 + 30×60 = 19800
NTPClient timeClient(ntpUDP, "asia.pool.ntp.org", 19800, 60000);
 
char Time[ ] = "TIME:00:00:00";
char Date[ ] = "DATE:00/00/2000";
byte last_second, second_, minute_, hour_, day_, month_;
int year_;
 
 
 
void setup() {
 
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  Serial.print("Connecting.");
 
  while ( WiFi.status() != WL_CONNECTED ) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("connected");
  timeClient.begin();

   while (!SD.begin(CS)) {
    Serial.println("SD card initialization failed!");
    delay(1000);
  }

  delay(1000);
  Serial.println("SD card initialization done.");
  SD.remove("NewFile.txt");
}
 
 
void loop() {
 
  timeClient.update();
  unsigned long unix_epoch = timeClient.getEpochTime();    // Get Unix epoch time from the NTP server
 
  second_ = second(unix_epoch);
  if (last_second != second_) {
 
 
    minute_ = minute(unix_epoch);
    hour_   = hour(unix_epoch);
    day_    = day(unix_epoch);
    month_  = month(unix_epoch);
    year_   = year(unix_epoch);
 
 
 
    Time[12] = second_ % 10 + 48;
    Time[11] = second_ / 10 + 48;
    Time[9]  = minute_ % 10 + 48;
    Time[8]  = minute_ / 10 + 48;
    Time[6]  = hour_   % 10 + 48;
    Time[5]  = hour_   / 10 + 48;
 
 
 
    Date[5]  = day_   / 10 + 48;
    Date[6]  = day_   % 10 + 48;
    Date[8]  = month_  / 10 + 48;
    Date[9]  = month_  % 10 + 48;
    Date[13] = (year_   / 10) % 10 + 48;
    Date[14] = year_   % 10 % 10 + 48;
 
    Serial.println(Time);
    Serial.println(Date);
 
    
    last_second = second_;
 
  }
  char formattedData[50];
  int randomNumber = random(100, 1000);
  sprintf(formattedData, "%lu-%s-%s", SerialNumber, Date, Time);
  WriteFile("/NewFile.txt", formattedData);
  SerialNumber++;
  delay(1000);
}
void WriteFile(const char *path, const char *message) {
  myFile = SD.open(path, FILE_WRITE);
  if (myFile) {
    myFile.println(message);
    myFile.close();
  } else {
    Serial.println("error opening file");
  }
}