//https://lastminuteengineers.com/esp8266-ntp-server-date-time-tutorial/

#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const char *ssid     = "DW";
const char *password = "27842821";

// UTC+08:00
const long utcOffsetInSeconds = 3600 * 8;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

void setup(){
  Serial.begin(115200);

  WiFi.begin(ssid, password);

  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }
  Serial.println();

  timeClient.begin();
}

void loop() {
  timeClient.update();

  //Serial.print(daysOfTheWeek[timeClient.getDay()] + " ");
  Serial.println(timeClient.getFormattedTime());

  delay(1000);
}
